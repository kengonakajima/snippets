#include <stdio.h>
#include <sys/time.h>

#include "include/opus.h"

/*

  https://github.com/hraban/libopus/blob/master/doc/trivial_example.c

*/

inline double now() {
    struct timeval tmv;
    gettimeofday( &tmv, NULL );
    return tmv.tv_sec  + (double)(tmv.tv_usec) / 1000000.0f;
}


int main() {
    OpusEncoder *encoder;
    OpusDecoder *decoder;
    int err;


    encoder = opus_encoder_create(24000,1, OPUS_APPLICATION_VOIP,&err);
    fprintf(stderr, "encoder:%p\n",encoder);
    err = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(30000));
    fprintf(stderr, "set_bitrate:%d\n",err);

    decoder = opus_decoder_create(24000,1,&err);
    fprintf(stderr, "decoder:%p err:%d\n",decoder,err);

    FILE *fp = fopen("short_24k_haikei.pcm","r");
    
    const int U=480;

    double t0 = now();
    int cnt=0;
    while(1) {
        char buf[U*2];
        size_t sz=fread(buf,1,U*2,fp);
        if(sz<U*2)break;
        short *samples = (short*)buf;
        unsigned char encoded[4096];
        int nbytes=opus_encode(encoder,samples,U,encoded,sizeof(encoded));
        //        fprintf(stderr, "encbytes:%d\n",nbytes);
        short decoded[U*10];
        int frame_size = opus_decode(decoder,encoded,nbytes,decoded,U*10,0);
        cnt++;
    }
    double t1=now();
    fprintf(stderr, "DT:%f cnt:%d\n",(t1-t0),cnt);
    fclose(fp);
    return 0;
}
