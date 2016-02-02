// tutorial05.c
// A pedagogical video player that really works!
//
// This tutorial was written by Stephen Dranger (dranger@gmail.com) and updated
// for ffmpeg version N-42806-gf4451d2 by Michael Penkov 
// (misha.penkov@gmail.com). 
//
// Code based on FFplay, Copyright (c) 2003 Fabrice Bellard, 
// and a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1
//
// Use the Makefile to build all the samples.
//
// Run using
// tutorial05 myvideofile.mpg
//
// to play the video.

// http://dranger.com/ffmpeg/tutorial05.html
/*
 * (1). main()->SDL_CreateThread(decode_thread, is), then main thread will wait for event to handle. new thread decode_thread will do the following things.
 * (2). stream_component_open(audio), stream_component_open(video), in stream_component_open, we will set audio callback, find the right audio/video decoder, packet_queue_init, SDL_CreateThread(video_thread, is).
 * (3). then we will enter main decode loop for(;;). In main decode loop, we will av_read_frame(), packet_queue_put()
 * (4). in stream_component_open(), we will set audio_callback(void *userdata, Uint8 *stream, int len), audio_decode_frame, memcpy decoded data to stream, then SDL will render the audio raw data. packet_queue_init(audio)
 * (5). in stream_component_open(), we will SDL_CreateThread(video_thread, is); codecCtx->get_buffer = our_get_buffer; codecCtx->release_buffer = our_release_buffer;
 * (6). video_thread, packet_queue_get(), avcodec_decode_video2(), synchronize_video()--->it will return pts. queue_picture()
 * (7). schedule_refresh(), FF_REFRESH_EVENT, video_refresh_timer()
 * (8). get_audio_clock(), return audio pts.
 * (9). video_refresh_timer(), the main implementation for A/V sync. There is a tons of code here.
 * (10). There is a bug in this code, we can not quit after finish playback a file. It's due to the implementation mechanism, can not fix it unless we make a log of changes.
 */

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavutil/avstring.h>

#include <SDL.h>
#include <SDL_thread.h>

#ifdef __MINGW32__
#undef main /* Prevents SDL from overriding main() */
#endif

#include <stdio.h>
#include <math.h>

#define SDL_AUDIO_BUFFER_SIZE 1024

#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)
#define MAX_VIDEOQ_SIZE (5 * 256 * 1024)

#define AV_SYNC_THRESHOLD 0.01
#define AV_NOSYNC_THRESHOLD 10.0

#define FF_ALLOC_EVENT   (SDL_USEREVENT)
#define FF_REFRESH_EVENT (SDL_USEREVENT + 1)
#define FF_QUIT_EVENT (SDL_USEREVENT + 2)

#define VIDEO_PICTURE_QUEUE_SIZE 1

typedef struct PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    SDL_mutex *mutex;
    SDL_cond *cond;
} PacketQueue;


typedef struct VideoPicture {
    SDL_Overlay *bmp;
    int width, height; /* source height & width */
    int allocated;
    double pts;
} VideoPicture;

typedef struct VideoState {

    AVFormatContext *pFormatCtx;
    int             videoStream, audioStream;

    double          audio_clock;
    AVStream        *audio_st;
    PacketQueue     audioq;
    AVFrame         audio_frame;
    uint8_t         audio_buf[(192000 * 3) / 2];
    unsigned int    audio_buf_size;
    unsigned int    audio_buf_index;
    AVPacket        audio_pkt;
    uint8_t         *audio_pkt_data;
    int             audio_pkt_size;
    int             audio_hw_buf_size;  
    double          frame_timer;
    double          frame_last_pts;
    double          frame_last_delay;
    double          video_clock; ///<pts of last decoded frame / predicted pts of next decoded frame
    AVStream        *video_st;
    PacketQueue     videoq;

    VideoPicture    pictq[VIDEO_PICTURE_QUEUE_SIZE];
    int             pictq_size, pictq_rindex, pictq_windex;
    SDL_mutex       *pictq_mutex;
    SDL_cond        *pictq_cond;

    SDL_Thread      *parse_tid;
    SDL_Thread      *video_tid;

    char            filename[1024];
    int             quit;

    AVIOContext     *io_context;
    struct SwsContext *sws_ctx;
} VideoState;

SDL_Surface     *screen;

/* Since we only have one decoding thread, the Big Struct
   can be global in case we need it. */
VideoState *global_video_state;

void packet_queue_init(PacketQueue *q) {
    memset(q, 0, sizeof(PacketQueue));
    q->mutex = SDL_CreateMutex();
    q->cond = SDL_CreateCond();
}

int packet_queue_put(PacketQueue *q, AVPacket *pkt) {

    AVPacketList *pkt1;
    if(av_dup_packet(pkt) < 0) {
        return -1;
    }
    pkt1 = av_malloc(sizeof(AVPacketList));
    if (!pkt1)
        return -1;
    pkt1->pkt = *pkt;
    pkt1->next = NULL;

    SDL_LockMutex(q->mutex);

    if (!q->last_pkt)
        q->first_pkt = pkt1;
    else
        q->last_pkt->next = pkt1;
    q->last_pkt = pkt1;
    q->nb_packets++;
    q->size += pkt1->pkt.size;
    SDL_CondSignal(q->cond);

    SDL_UnlockMutex(q->mutex);
    return 0;
}
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
{
    AVPacketList *pkt1;
    int ret;

    SDL_LockMutex(q->mutex);

    for(;;) {
        if(global_video_state->quit) {
            ret = -1;
            break;
        }

        pkt1 = q->first_pkt;
        if (pkt1) {
            q->first_pkt = pkt1->next;
            if (!q->first_pkt)
                q->last_pkt = NULL;
            q->nb_packets--;
            q->size -= pkt1->pkt.size;
            *pkt = pkt1->pkt;
            av_free(pkt1);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            /*
             * FIXME: Bug here, can not quit after finish playback.
             * We do not know whether there is no packet in queue, or the producer is too slow to put packet in queue.
             * So we can not just return -1 here. Maybe after wait for a few ms, we can get packet again.
             */
            SDL_CondWait(q->cond, q->mutex);
        }
    }
    SDL_UnlockMutex(q->mutex);
    return ret;
}
double get_audio_clock(VideoState *is) {
    double pts;
    int hw_buf_size, bytes_per_sec, n;

    pts = is->audio_clock; /* maintained in the audio thread */
    hw_buf_size = is->audio_buf_size - is->audio_buf_index;
    bytes_per_sec = 0;
    n = is->audio_st->codec->channels * 2;
    if(is->audio_st) {
        bytes_per_sec = is->audio_st->codec->sample_rate * n;
    }
    if(bytes_per_sec) {
        pts -= (double)hw_buf_size / bytes_per_sec;
    }
    fprintf(stderr, "audio clock, pts: %.8f\n", pts);
    return pts;
}

int audio_decode_frame(VideoState *is, double *pts_ptr) {
    int len1, data_size = 0, n;
    AVPacket *pkt = &is->audio_pkt;
    double pts;

    for(;;) {
        while(is->audio_pkt_size > 0) {
            int got_frame;
            len1 = avcodec_decode_audio4(is->audio_st->codec, &is->audio_frame, &got_frame, pkt);
            if(len1 < 0) {
                /* if error, skip frame */
                is->audio_pkt_size = 0;
                break;
            }
            if (got_frame)
                {
                    data_size = 
                        av_samples_get_buffer_size
                        (
                         NULL, 
                         is->audio_st->codec->channels,
                         is->audio_frame.nb_samples,
                         is->audio_st->codec->sample_fmt,
                         1
                         );
                    memcpy(is->audio_buf, is->audio_frame.data[0], data_size);
                }
            is->audio_pkt_data += len1;
            is->audio_pkt_size -= len1;
            if(data_size <= 0) {
                /* No data yet, get more frames */
                fprintf(stderr, "%s, No data yet, get more frames\n", __FUNCTION__);
                continue;
            }
            pts = is->audio_clock;
            *pts_ptr = pts;
            n = 2 * is->audio_st->codec->channels;
            is->audio_clock += (double)data_size /
                (double)(n * is->audio_st->codec->sample_rate);

            /* We have data, return it and come back for more later */
            return data_size;
        }
        if(pkt->data)
            av_free_packet(pkt);

        if(is->quit) {
			fprintf(stderr, "%s, is->quit\n", __FUNCTION__);
            return -1;
        }
        /* next packet */
        if(packet_queue_get(&is->audioq, pkt, 1) < 0) {
			fprintf(stderr, "can not get packet from audio queue\n");
            return -1;
        }
        is->audio_pkt_data = pkt->data;
        is->audio_pkt_size = pkt->size;
        /* if update, update the audio clock w/pts */
        if(pkt->pts != AV_NOPTS_VALUE) {
            is->audio_clock = av_q2d(is->audio_st->time_base)*pkt->pts;
        }

    }
}

void audio_callback(void *userdata, Uint8 *stream, int len) {

    VideoState *is = (VideoState *)userdata;
    int len1, audio_size;
    double pts;

    while(len > 0) {
        if(is->audio_buf_index >= is->audio_buf_size) {
            /* We have already sent all our data; get more */
            audio_size = audio_decode_frame(is, &pts);
            if(audio_size < 0) {
                /* If error, output silence */
                is->audio_buf_size = 1024;
                memset(is->audio_buf, 0, is->audio_buf_size);
            } else {
                is->audio_buf_size = audio_size;
            }
            is->audio_buf_index = 0;
        }
        len1 = is->audio_buf_size - is->audio_buf_index;
        if(len1 > len)
            len1 = len;
        memcpy(stream, (uint8_t *)is->audio_buf + is->audio_buf_index, len1);
        len -= len1;
        stream += len1;
        is->audio_buf_index += len1;
    }
}

static Uint32 sdl_refresh_timer_cb(Uint32 interval, void *opaque) {
    SDL_Event event;
    event.type = FF_REFRESH_EVENT;
    event.user.data1 = opaque;
    SDL_PushEvent(&event);
    return 0; /* 0 means stop timer */
}

/* schedule a video refresh in 'delay' ms */
static void schedule_refresh(VideoState *is, int delay) {
    SDL_AddTimer(delay, sdl_refresh_timer_cb, is);
}

void video_display(VideoState *is) {

    SDL_Rect rect;
    VideoPicture *vp;
    float aspect_ratio;
    int w, h, x, y;
    
    vp = &is->pictq[is->pictq_rindex];
    if(vp->bmp) {
        if(is->video_st->codec->sample_aspect_ratio.num == 0) {
            aspect_ratio = 0;
        } else {
            aspect_ratio = av_q2d(is->video_st->codec->sample_aspect_ratio) *
                is->video_st->codec->width / is->video_st->codec->height;
        }
        if(aspect_ratio <= 0.0) {
            aspect_ratio = (float)is->video_st->codec->width /
                (float)is->video_st->codec->height;
        }
        h = screen->h;
        w = ((int)rint(h * aspect_ratio)) & -3;
        if(w > screen->w) {
            w = screen->w;
            h = ((int)rint(w / aspect_ratio)) & -3;
        }
        x = (screen->w - w) / 2;
        y = (screen->h - h) / 2;

        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        SDL_DisplayYUVOverlay(vp->bmp, &rect);
    }
}

void video_refresh_timer(void *userdata) {

    VideoState *is = (VideoState *)userdata;
    VideoPicture *vp;
    double actual_delay, delay, sync_threshold, ref_clock, diff;

    if(is->video_st) {
        if(is->pictq_size == 0) {
            fprintf(stderr, "%s pictq_size is 0, schedule another refresh\n", __FUNCTION__);
            schedule_refresh(is, 1);
        } else {
            vp = &is->pictq[is->pictq_rindex];

            delay = vp->pts - is->frame_last_pts; /* the pts from last time */
            fprintf(stderr, "delay 1: %.8f\n", delay);
            if(delay <= 0 || delay >= 1.0) { //larger than 1 seconds or smaller than 0
                /* if incorrect delay, use previous one */
                delay = is->frame_last_delay;
                fprintf(stderr, "delay 2: %.8f\n", delay);
            }
            /* save for next time */
            is->frame_last_delay = delay;
            is->frame_last_pts = vp->pts;

            /* update delay to sync to audio */
            ref_clock = get_audio_clock(is);
            diff = vp->pts - ref_clock;
            fprintf(stderr, "audio video diff: %.8f\n", diff);

            /* Skip or repeat the frame. Take delay into account
               FFPlay still doesn't "know if this is the best guess." */
            sync_threshold = (delay > AV_SYNC_THRESHOLD) ? delay : AV_SYNC_THRESHOLD;
            if(fabs(diff) < AV_NOSYNC_THRESHOLD) {
                if(diff <= -sync_threshold) {
                    delay = 0;
                } else if(diff >= sync_threshold) {
                    delay = 2 * delay;
                }
            }
            is->frame_timer += delay;
            /* computer the REAL delay */
            actual_delay = is->frame_timer - (av_gettime() / 1000000.0);
            fprintf(stderr, "actual_delay %.8f\n", actual_delay);
            if(actual_delay < 0.010) { //smaller than 10 ms
                /* Really it should skip the picture instead */
                actual_delay = 0.010;
            }
            // Why add 0.5 here. I see many 0.5 in multimedia framework code, such as stagefright.
            fprintf(stderr, "%s, delay: %.8f\n", __FUNCTION__, actual_delay*1000+0.5);
            // Video is faster than audio, so we need a delay render.
            // after we show a frame, we figure out when the next frame should be shown
            schedule_refresh(is, (int)(actual_delay * 1000 + 0.5)); 
            /* show the picture! */
            video_display(is);
            fprintf(stderr, "\n---------------------------------------------------------------------\n");

            /* update queue for next picture! */
            if(++is->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE) {
                is->pictq_rindex = 0;
            }
            SDL_LockMutex(is->pictq_mutex);
            is->pictq_size--;
            SDL_CondSignal(is->pictq_cond);
            SDL_UnlockMutex(is->pictq_mutex);
        }
    } else {
        fprintf(stderr, "%s, schedule_refresh for another 100 ms\n", __FUNCTION__);
        schedule_refresh(is, 100);
    }
}

void alloc_picture(void *userdata) {

    VideoState *is = (VideoState *)userdata;
    VideoPicture *vp;

    vp = &is->pictq[is->pictq_windex];
    if(vp->bmp) {
        // we already have one make another, bigger/smaller
        SDL_FreeYUVOverlay(vp->bmp);
    }
    // Allocate a place to put our YUV image on that screen
    vp->bmp = SDL_CreateYUVOverlay(is->video_st->codec->width,
                                   is->video_st->codec->height,
                                   SDL_YV12_OVERLAY,
                                   screen);
    vp->width = is->video_st->codec->width;
    vp->height = is->video_st->codec->height;

    SDL_LockMutex(is->pictq_mutex);
    vp->allocated = 1;
    SDL_CondSignal(is->pictq_cond);
    SDL_UnlockMutex(is->pictq_mutex);

}

int queue_picture(VideoState *is, AVFrame *pFrame, double pts) {

    VideoPicture *vp;
    AVPicture pict;

    /* wait until we have space for a new pic */
    SDL_LockMutex(is->pictq_mutex);
    while(is->pictq_size >= VIDEO_PICTURE_QUEUE_SIZE &&
          !is->quit) {
        SDL_CondWait(is->pictq_cond, is->pictq_mutex);
    }
    SDL_UnlockMutex(is->pictq_mutex);

    if(is->quit) {
        fprintf(stderr, "%s, is->quit", __FUNCTION__);
        return -1;
    }
        

    // windex is set to 0 initially
    vp = &is->pictq[is->pictq_windex];

    /* allocate or resize the buffer! */
    if(!vp->bmp ||
       vp->width != is->video_st->codec->width ||
       vp->height != is->video_st->codec->height) {
        SDL_Event event;

        vp->allocated = 0;
        /* we have to do it in the main thread */
        event.type = FF_ALLOC_EVENT;
        event.user.data1 = is;
        SDL_PushEvent(&event);

        /* wait until we have a picture allocated */
        SDL_LockMutex(is->pictq_mutex);
        while(!vp->allocated && !is->quit) {
            SDL_CondWait(is->pictq_cond, is->pictq_mutex);
        }
        SDL_UnlockMutex(is->pictq_mutex);
        if(is->quit) {
            return -1;
        }
    }
    /* We have a place to put our picture on the queue */
    /* If we are skipping a frame, do we set this to null 
       but still return vp->allocated = 1? */


    if(vp->bmp) {

        SDL_LockYUVOverlay(vp->bmp);

        /* point pict at the queue */

        pict.data[0] = vp->bmp->pixels[0];
        pict.data[1] = vp->bmp->pixels[2];
        pict.data[2] = vp->bmp->pixels[1];

        pict.linesize[0] = vp->bmp->pitches[0];
        pict.linesize[1] = vp->bmp->pitches[2];
        pict.linesize[2] = vp->bmp->pitches[1];

        // Convert the image into YUV format that SDL uses
        sws_scale
            (
             is->sws_ctx,
             (uint8_t const * const *)pFrame->data,
             pFrame->linesize,
             0, 
             is->video_st->codec->height, 
             pict.data, 
             pict.linesize
             );

        SDL_UnlockYUVOverlay(vp->bmp);
        vp->pts = pts;

        /* now we inform our display thread that we have a pic ready */
        if(++is->pictq_windex == VIDEO_PICTURE_QUEUE_SIZE) {
            is->pictq_windex = 0;
        }
        SDL_LockMutex(is->pictq_mutex);
        is->pictq_size++;
        SDL_UnlockMutex(is->pictq_mutex);
    }
    return 0;
}

/* 
 * we account for repeated frames in this function
 * ?
 * Can not understand what is repeated frames.
 */
double synchronize_video(VideoState *is, AVFrame *src_frame, double pts) {
    
    double frame_delay;

    if(pts != 0) {
        /* if we have pts, set video clock to it */
        is->video_clock = pts;
    } else {
        /* if we aren't given a pts, set it to the clock */
        pts = is->video_clock;
    }
    /* update the video clock */
    frame_delay = av_q2d(is->video_st->codec->time_base);
    /* if we are repeating a frame, adjust clock accordingly */
    frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
    is->video_clock += frame_delay;
    // fprintf(stderr, "%s, pts: %.8f\n", __FUNCTION__, pts);
    return pts;
}
uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

/* These are called whenever we allocate a frame
 * buffer. We use this to store the global_pts in
 * a frame at the time it is allocated.
 */
int our_get_buffer(struct AVCodecContext *c, AVFrame *pic) {
    int ret = avcodec_default_get_buffer(c, pic);
    uint64_t *pts = av_malloc(sizeof(uint64_t));
    // We set the wrong value here: global_video_pkt_pts
    *pts = global_video_pkt_pts;
    pic->opaque = pts;
    return ret;
}

void our_release_buffer(struct AVCodecContext *c, AVFrame *pic) {
    if(pic) {
        av_freep(&pic->opaque);
    }
    avcodec_default_release_buffer(c, pic);
}

int video_thread(void *arg) {
    VideoState *is = (VideoState *)arg;
    AVPacket pkt1, *packet = &pkt1;
    int frameFinished;
    AVFrame *pFrame;
    double pts;

    pFrame = avcodec_alloc_frame();

    for(;;) {
        if(packet_queue_get(&is->videoq, packet, 1) < 0) {
            // means we quit getting packets
            fprintf(stderr, "%s, can not get packet from video queue\n", __FUNCTION__);
            break;
        }
        pts = 0;

        // Save global pts to be stored in pFrame in first call
        global_video_pkt_pts = packet->pts;
        // Decode video frame
        avcodec_decode_video2(is->video_st->codec, pFrame, &frameFinished, 
                              packet);
        if(packet->dts == AV_NOPTS_VALUE 
           && pFrame->opaque && *(uint64_t*)pFrame->opaque != AV_NOPTS_VALUE) {
            pts = *(uint64_t *)pFrame->opaque;
        } else if(packet->dts != AV_NOPTS_VALUE) {
            pts = packet->dts;
        } else {
            pts = 0;
        }
        pts *= av_q2d(is->video_st->time_base);

        // Did we get a video frame?
        if(frameFinished) {
            pts = synchronize_video(is, pFrame, pts);
            if(queue_picture(is, pFrame, pts) < 0) {
                fprintf(stderr, "%s, can not queue_picture\n", __FUNCTION__);
                break;
            }
        }
        av_free_packet(packet);
    }
    av_free(pFrame);
    return 0;
}

int stream_component_open(VideoState *is, int stream_index) {

    AVFormatContext *pFormatCtx = is->pFormatCtx;
    AVCodecContext *codecCtx = NULL;
    AVCodec *codec = NULL;
    AVDictionary *optionsDict = NULL;
    SDL_AudioSpec wanted_spec, spec;

    if(stream_index < 0 || stream_index >= pFormatCtx->nb_streams) {
        return -1;
    }

    // Get a pointer to the codec context for the video stream
    codecCtx = pFormatCtx->streams[stream_index]->codec;

    if(codecCtx->codec_type == AVMEDIA_TYPE_AUDIO) {
        // Set audio settings from codec info
        wanted_spec.freq = codecCtx->sample_rate;
        wanted_spec.format = AUDIO_S16SYS;
        wanted_spec.channels = codecCtx->channels;
        wanted_spec.silence = 0;
        wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
        wanted_spec.callback = audio_callback;
        wanted_spec.userdata = is;

        if(SDL_OpenAudio(&wanted_spec, &spec) < 0) {
            fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
            return -1;
        }
        is->audio_hw_buf_size = spec.size;
    }
    codec = avcodec_find_decoder(codecCtx->codec_id);

    if(!codec || (avcodec_open2(codecCtx, codec, &optionsDict) < 0)) {
        fprintf(stderr, "Unsupported codec!\n");
        return -1;
    }

    switch(codecCtx->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        is->audioStream = stream_index;
        is->audio_st = pFormatCtx->streams[stream_index];
        is->audio_buf_size = 0;
        is->audio_buf_index = 0;
        memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));
        packet_queue_init(&is->audioq);
        SDL_PauseAudio(0);
        break;

    case AVMEDIA_TYPE_VIDEO:
        is->videoStream = stream_index;
        is->video_st = pFormatCtx->streams[stream_index];

        is->frame_timer = (double)av_gettime() / 1000000.0;
        is->frame_last_delay = 40e-3;

        packet_queue_init(&is->videoq);
        is->video_tid = SDL_CreateThread(video_thread, is);
        is->sws_ctx =
            sws_getContext
            (
             is->video_st->codec->width,
             is->video_st->codec->height,
             is->video_st->codec->pix_fmt,
             is->video_st->codec->width,
             is->video_st->codec->height,
             PIX_FMT_YUV420P, 
             SWS_BILINEAR, 
             NULL, 
             NULL, 
             NULL
             );
        codecCtx->get_buffer = our_get_buffer;
        codecCtx->release_buffer = our_release_buffer;
        break;

    default:
        break;
    }

    return 0;
}

int decode_interrupt_cb(void *opaque) {
    return (global_video_state && global_video_state->quit);
}

int decode_thread(void *arg) {

    VideoState *is = (VideoState *)arg;
    AVFormatContext *pFormatCtx = NULL;
    AVPacket pkt1, *packet = &pkt1;

    AVDictionary *io_dict = NULL;
    AVIOInterruptCB callback;

    int video_index = -1;
    int audio_index = -1;
    int i;

    is->videoStream=-1;
    is->audioStream=-1;

    global_video_state = is;
    // will interrupt blocking functions if we quit!
    callback.callback = decode_interrupt_cb;
    callback.opaque = is;
    if (avio_open2(&is->io_context, is->filename, 0, &callback, &io_dict))
        {
            fprintf(stderr, "Unable to open I/O for %s\n", is->filename);
            return -1;
        }

    // Open video file
    if(avformat_open_input(&pFormatCtx, is->filename, NULL, NULL)!=0)
        return -1; // Couldn't open file

    is->pFormatCtx = pFormatCtx;

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
        return -1; // Couldn't find stream information

    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, is->filename, 0);

    // Find the first video stream

    for(i=0; i<pFormatCtx->nb_streams; i++) {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO &&
           video_index < 0) {
            video_index=i;
        }
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO &&
           audio_index < 0) {
            audio_index=i;
        }
    }
    if(audio_index >= 0) {
        stream_component_open(is, audio_index);
    }
    if(video_index >= 0) {
        stream_component_open(is, video_index);
    }   

    if(is->videoStream < 0 || is->audioStream < 0) {
        fprintf(stderr, "%s: could not open codecs\n", is->filename);
        goto fail;
    }

    // main decode loop
    for(;;) {
        if(is->quit) {
            break;
        }
        // seek stuff goes here
        if(is->audioq.size > MAX_AUDIOQ_SIZE ||
           is->videoq.size > MAX_VIDEOQ_SIZE) {
            SDL_Delay(10);
            continue;
        }
        if(av_read_frame(is->pFormatCtx, packet) < 0) {
            if(is->pFormatCtx->pb->error == 0) {
                SDL_Delay(100); /* no error; wait for user input */
                continue;
            } else {
                break;
            }
        }
        // Is this a packet from the video stream?
        if(packet->stream_index == is->videoStream) {
            packet_queue_put(&is->videoq, packet);
        } else if(packet->stream_index == is->audioStream) {
            packet_queue_put(&is->audioq, packet);
        } else {
            av_free_packet(packet);
        }
    }
    /* all done - wait for it */
    while(!is->quit) {
        SDL_Delay(100);
    }

 fail:
    {
        SDL_Event event;
        event.type = FF_QUIT_EVENT;
        event.user.data1 = is;
        SDL_PushEvent(&event);
    }
    return 0;
}

int main(int argc, char *argv[]) {

    SDL_Event       event;

    VideoState      *is;

    is = av_mallocz(sizeof(VideoState));

    if(argc < 2) {
        fprintf(stderr, "Usage: %s filepath\n", argv[0]);
        exit(1);
    }
    // Register all formats and codecs
    av_register_all();

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        exit(1);
    }

    // Make a screen to put our video
#ifndef __DARWIN__
    screen = SDL_SetVideoMode(640, 480, 0, 0);
#else
    screen = SDL_SetVideoMode(640, 480, 24, 0);
#endif
    if(!screen) {
        fprintf(stderr, "SDL: could not set video mode - exiting\n");
        exit(1);
    }

    av_strlcpy(is->filename, argv[1], 1024);

    is->pictq_mutex = SDL_CreateMutex();
    is->pictq_cond = SDL_CreateCond();

    schedule_refresh(is, 40);

    is->parse_tid = SDL_CreateThread(decode_thread, is);
    if(!is->parse_tid) {
        av_free(is);
        return -1;
    }
    for(;;) {

        SDL_WaitEvent(&event);
        switch(event.type) {
        case FF_QUIT_EVENT:
        case SDL_QUIT:
            is->quit = 1;
            /*
             * If the video has finished playing, then both the picture and
             * audio queues are waiting for more data.  Make them stop
             * waiting and terminate normally.
             */
            SDL_CondSignal(is->audioq.cond);
            SDL_CondSignal(is->videoq.cond);
            SDL_Quit();
            exit(0);
            break;

        case FF_ALLOC_EVENT:
            alloc_picture(event.user.data1);
            break;

        case FF_REFRESH_EVENT:
            video_refresh_timer(event.user.data1);
            break;

        default:
            break;
        }
    }
    return 0;

}
