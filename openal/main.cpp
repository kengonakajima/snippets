#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <iostream>
#include <stdio.h>

using namespace std;

const int SRATE = 44100;
const int SSIZE = 4096;

ALbyte data[22050];
ALint sample;



int main(int argc, char *argv[]) {
    alGetError();

    // record
    alGetError();
    ALCdevice *device = alcCaptureOpenDevice(NULL, SRATE, AL_FORMAT_STEREO16, SSIZE);
    if (alGetError() != AL_NO_ERROR) {
        return 0;
    }
    alcCaptureStart(device);

    int val;

    int cnt=0;
    
    while (true) {
        alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);
        alcCaptureSamples(device, (ALCvoid *)data, sample);

        if( sample > 0 ){
            short sbuf[100],maxvol;
            int i;
            maxvol=0;
            for(i=0;i<20;i++){
                sbuf[i] = * ( (short*)(data+i*2) );
                if( sbuf[i] < 0 ){ sbuf[i] *= -1; }
                if( sbuf[i] > maxvol ) maxvol = sbuf[i];
            }
            fprintf(stderr,"%d %04x ", cnt, maxvol );
            for(i=0;i<maxvol/1000;i++){ 
                fprintf(stderr,".");
            }
            fprintf(stderr,"\n");

            char path[100];
            snprintf(path,sizeof(path), "./recorded/snd_%d.wav", cnt );
            cnt++;
            FILE *fp = fopen(path,"w");
            // http://www.kk.iij4u.or.jp/~kondo/wave/
            char header[40] = {
                0x52,0x49,0x46,0x46, // RIFF
                0,0,0,0, // fileLen
                0x57,0x41,0x56,0x45, // WAVE
                0x66,0x6D,0x74,0x20, // "fmt "
                0x10,0,0,0, // 16
                0x01,0, // 1 (format ID)
                0x02,0, // 2 channel
                0x44,0xac, 0,0, // 44100
                0x10,0xb1, 0x02, 0, // 176400 bit/sec データ速度
                0x04, 0, // 4 (block size = 2x2)
                0x10, 0, // 16 (bit depth/sample)
                0x64,0x61,0x74,0x61, //  "data"
            };
            fwrite(header,1,40,fp);
            int datalen = sample * 4;
            fwrite(&datalen,4,1,fp);
            fwrite( data, 4, sample, fp );            
            fseek(fp, 4, SEEK_SET);
            int filelen = sample*4 + 44 - 8;
            fwrite( &filelen, 4,1,fp);
            fclose(fp);
        }
    }

    alcCaptureStop(device);
    alcCaptureCloseDevice(device);

    return 0;
}
