#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <iostream>
#include <stdio.h>

using namespace std;

const int SRATE = 44100;
const int SSIZE = 2048;

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
            fprintf(stderr,"%04x ", maxvol );
            for(i=0;i<maxvol/1000;i++){ 
                fprintf(stderr,".");
            }
            fprintf(stderr,"\n");

            char path[100];
            snprintf(path,sizeof(path), "./recorded/snd_%d.dat", cnt );
            cnt++;
            FILE *fp = fopen(path,"w");
            fwrite( data, 1, sample, fp );
            fclose(fp);
        
        }
    }

    alcCaptureStop(device);
    alcCaptureCloseDevice(device);

    return 0;
}
