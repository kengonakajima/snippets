#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "util.h"

float to_f(short s) {
    float f=(float)s;
    f/=32767.0;
    return f;
}
short to_s(float f) {
    return (short)(f*32767.0);
}

void dump_samples_s(short *buf, int n) {
    for(int i=0;i<n;i++) printf("%d ", buf[i]);
    printf("\n");
}
void dump_samples_f(float *buf, int n) {
    for(int i=0;i<n;i++) printf("%1.8f ", buf[i]);
    printf("\n");
}
void dump_complex_f(std::complex<float>ary[],int n) {
    for(int i=0;i<n;i++) {
        printf("%f %f\n",ary[i].real(),ary[i].imag());
    }
    
}
void save_s(short *buf,int n, const char *path) {
    FILE *fp=fopen(path,"w");
    fwrite(buf,1,n*2,fp);
    fclose(fp);
}
void save_f(float *buf,int n, const char *path) {
    short *sb=(short*)malloc(n*2);
    for(int i=0;i<n;i++) sb[i]=to_s(buf[i]);
    FILE *fp=fopen(path,"w");
    fwrite(sb,1,n*2,fp);
    fclose(fp);
    free(sb);
}


float calc_mse(float *error_signal, int signal_length) {
    float sum_squared_error = 0.0f;
    for (int i = 0; i < signal_length; i++) {
        sum_squared_error += error_signal[i] * error_signal[i];
    }
    return sum_squared_error / signal_length;
}

