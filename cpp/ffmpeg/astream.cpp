/*
 * Copyright (c) 2001 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * audio encoding with libavcodec API example.
 *
 * @example encode_audio.c
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
    
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
};

AVStream *audioStream;
AVFormatContext *ofmt_ctx;

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt, const char *tag) {
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    printf("%s: pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           tag,
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}


/* check that a given sample format is supported by the encoder */
static int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

/* just pick the highest supported samplerate */
static int select_sample_rate(const AVCodec *codec)
{
    const int *p;
    int best_samplerate = 0;

    if (!codec->supported_samplerates)
        return 44100;

    p = codec->supported_samplerates;
    while (*p) {
        if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
            best_samplerate = *p;
        p++;
    }
    return best_samplerate;
}

/* select layout with the highest channel count */
static int select_channel_layout(const AVCodec *codec)
{
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels   = 0;

    if (!codec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;

    p = codec->channel_layouts;
    while (*p) {
        int nb_channels = av_get_channel_layout_nb_channels(*p);

        if (nb_channels > best_nb_channels) {
            best_ch_layout    = *p;
            best_nb_channels = nb_channels;
        }
        p++;
    }
    return best_ch_layout;
}

static void encode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt)
{
    int ret;

    /* send the frame for encoding */
    ret = avcodec_send_frame(ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending the frame to the encoder\n");
        exit(1);
    }

    /* read all the available output packets (in general there may be any
     * number of them */
    while (ret >= 0) {
        ret = avcodec_receive_packet(ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error encoding audio frame\n");
            exit(1);
        }
        log_packet(ofmt_ctx,pkt,"audio");
        if (av_interleaved_write_frame(ofmt_ctx, pkt) != 0) {
            fprintf(stderr,"av_interleaved_write_frame audio failed\n");
        } else {
            fprintf(stderr,"av_interleaved_write_frame audio ok\n");
        }
        

        //     fwrite(pkt->data, 1, pkt->size, output);
        av_packet_unref(pkt);
    }
}


int main(int argc, char **argv)
{
    const char *filename;
    const AVCodec *audioCodec;
    AVCodecContext *audioCodecContext= NULL;
    AVFrame *frame;
    AVPacket *pkt;
    int i, j, k, ret;
    uint16_t *samples;
    float t, tincr;


    if (argc <= 1) {
        fprintf(stderr, "Usage: %s <output file>\n", argv[0]);
        return 0;
    }
    filename = argv[1];



    
    /* find the MP2 encoder */
    audioCodec = avcodec_find_encoder(AV_CODEC_ID_MP2);
    if (!audioCodec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    audioCodecContext = avcodec_alloc_context3(audioCodec);
    if (!audioCodecContext) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    /* put sample parameters */
    audioCodecContext->bit_rate = 64000;

    /* check that the encoder supports s16 pcm input */
    audioCodecContext->sample_fmt = AV_SAMPLE_FMT_S16;
    if (!check_sample_fmt(audioCodec, audioCodecContext->sample_fmt)) {
        fprintf(stderr, "Encoder does not support sample format %s",
                av_get_sample_fmt_name(audioCodecContext->sample_fmt));
        exit(1);
    }

    /* select other audio parameters supported by the encoder */
    audioCodecContext->sample_rate    = select_sample_rate(audioCodec);
    audioCodecContext->channel_layout = select_channel_layout(audioCodec);
    audioCodecContext->channels       = av_get_channel_layout_nb_channels(audioCodecContext->channel_layout);



    //        const char *outpath = "http://127.0.0.1:8081/hoge";  
    AVIOContext *io_context = nullptr;
    if( avio_open(&io_context,filename, AVIO_FLAG_WRITE)<0) {
        fprintf(stderr,"avio_open fail\n");
        return 1;
    }
    
    avformat_alloc_output_context2(&ofmt_ctx,NULL,"mpegts",nullptr);
    if(!ofmt_ctx) {
        fprintf(stderr, "avformat_alloc_output_context2 failed\n");
        return 1;
    }
    ofmt_ctx->pb = io_context;

    audioStream = avformat_new_stream(ofmt_ctx,audioCodec);
    if(!audioStream) {
        fprintf(stderr,"avformat_new_stream audio fail\n");
        return 1;
    }
    audioStream->time_base = audioCodecContext->time_base;
    fprintf(stderr,"audio stream index:%d\n", audioStream->index);
    if( avcodec_parameters_from_context(audioStream->codecpar, audioCodecContext)< 0 ) {
        fprintf(stderr,"avcodec_parameters_from_context for audio failed\n");
        return 1;
    }

    if (avformat_write_header(ofmt_ctx, nullptr) < 0) {
        fprintf(stderr,"avformat_write_header failed\n");
        return 1;
    }
    
    /* open it */
    if (avcodec_open2(audioCodecContext, audioCodec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    /* packet for holding encoded output */
    pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "could not allocate the packet\n");
        exit(1);
    }

    /* frame containing input raw audio */
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate audio frame\n");
        exit(1);
    }

    frame->nb_samples     = audioCodecContext->frame_size;
    frame->format         = audioCodecContext->sample_fmt;
    frame->channel_layout = audioCodecContext->channel_layout;

    /* allocate the data buffers */
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate audio data buffers\n");
        exit(1);
    }

    /* encode a single tone sound */
    t = 0;
    tincr = 2 * M_PI * 440.0 / audioCodecContext->sample_rate;
    for (i = 0; i < 200; i++) {
        /* make sure the frame is writable -- makes a copy if the encoder
         * kept a reference internally */
        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);
        samples = (uint16_t*)frame->data[0];

        for (j = 0; j < audioCodecContext->frame_size; j++) {
            samples[2*j] = (int)(sin(t) * 10000);

            for (k = 1; k < audioCodecContext->channels; k++)
                samples[2*j + k] = samples[2*j];
            t += tincr;
        }
        encode(audioCodecContext, frame, pkt);
    }

    /* flush the encoder */
    encode(audioCodecContext, NULL, pkt);



    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&audioCodecContext);

    avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    return 0;
}
