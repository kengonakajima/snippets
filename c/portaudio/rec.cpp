#include "portaudio.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SAMPLE_RATE (44100)


static int callback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    float *samples = (float*) inputBuffer;
    int n = samples[0] * 100;
    if(n>50)n=50;
    char bar[50+1];
    for(int i=0;i<n;i++) bar[i]='*';
    bar[n]=0;
    printf("volume: %s\n",bar);
    return 0;
}
void printErrorExit(PaError e) {
    const char *p=Pa_GetErrorText(e);    
    fprintf(stderr, "printErrorExit: %s\n",p);
    exit(1);
}

int main()
{
    PaError err;
    err=Pa_Initialize();
    if(err!=paNoError) printErrorExit(err);


    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) {
          printf("Error: No default input device.\n");
          Pa_Terminate();
          return 1;
    }
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    PaStream *stream;
    err = Pa_OpenStream( &stream, &inputParameters,
                         0,
                         Pa_GetDeviceInfo(inputParameters.device)->defaultSampleRate,
                         480,
                         paClipOff,
                         callback,
                         0
                         );
    if(err!=paNoError) printErrorExit(err);

    err = Pa_StartStream(stream);
    if(err!=paNoError) printErrorExit(err);
    while(1) {
        usleep(1000);
    }

    
    return 0;
}

    
