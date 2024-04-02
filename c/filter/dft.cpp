#include <complex.h>
#include <math.h>
#include "util.h"

using namespace std;

void dft(const complex<float> g[], complex<float> G[], int N) {
    complex<float> j2pn = complex<float>(0,-2*M_PI/N);
    for(int k=0;k<N;k++) {
        G[k]=0;
        for(int n=0;n<N;n++) {
            G[k]=G[k]+g[n]*exp(j2pn*(float)(n*k));
        }
    }
}

void idft(const complex<float> g[], complex<float> G[], int N) {
    complex<float> j2pn = complex<float>(0,2*M_PI/N);
    for(int k=0;k<N;k++) {
        G[k]=0;
        for(int n=0;n<N;n++) {
            G[k]=G[k]+g[n]*exp(j2pn*(float)(n*k));
        }
    }
    for(int k=0;k<N;k++) G[k]=G[k]/(float)N;
}



void generateSineWave(float* sineWave, float amplitude, float frequency, float duration, float sampleRate, int numSamples) {
    float angleIncrement = 2 * M_PI * frequency / sampleRate;
    float currentAngle = 0.0;

    for(int i = 0; i < numSamples; i++) {
        sineWave[i] = (float)(amplitude * sin(currentAngle));
        currentAngle += angleIncrement;
    }
}



int main() {
    const int N=1024;
    float samples[N];
    float freq=100;
    float sampleRate=24000;
    generateSineWave(samples,1,freq,N/freq,sampleRate,N);
    //printf("sine:\n");
    //dump_samples_f(samples,N);
    save_f(samples,N,"sine.raw");

    complex<float> g[N];
    for(int i=0;i<N;i++) {
        g[i]=complex<float>(samples[i],0);
    }
    complex<float> G[N];
    dft(g,G,N);
    printf("G:\n");
    for(int i=0;i<N;i++){
        float hz=sampleRate/N*i;
        printf("%d: %fHz: %f %f\n",i,hz,G[i].real(),G[i].imag());
    }
    //dump_complex_f(G,N);
    complex<float> rg[N];
    idft(G,rg,N);
    for(int i=0;i<N;i++){
        float diff=rg[i].real() - g[i].real();
        printf("%f %f    %f %f diff:%f\n",rg[i].real(),rg[i].imag(), g[i].real(),g[i].imag(),diff);
    }
    
}
