#include <math.h>
#include <complex>

float to_f(short s) ;
short to_s(float f) ;
void dump_samples_s(short *buf, int n) ;
void dump_samples_f(float *buf, int n) ;
void save_s(short *buf,int n, const char *path) ;
void save_f(float *buf,int n, const char *path) ;
float calc_mse(float *error_signal, int signal_length) ;
void dump_complex_f(std::complex<float>ary[],int n) ;
