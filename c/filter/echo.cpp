#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

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
    for(int i=0;i<n;i++) {
        printf("%1.8f ", buf[i]);
    }
    printf("\n");
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



///////////////////////////////////////////////


const int N=10;
// 鳴らした音
float played_samples[N]={
    0,0,1,0.5,0, 0,0,0,0,0
};
// マイクからまわりこんで来た音
float recorded_samples[N]={
    0,0,0,0,0, 0,0.2,0.1,0,0
};


#define FILTER_LENGTH N

// FIRフィルタの係数
float filter_coefficients[FILTER_LENGTH]; // 0から開始

// FIRフィルタの処理関数
float fir_filter(float* input_signal, int start_index) {
    float output = 0;

    // 畳み込み演算
    for (int i = 0; i < FILTER_LENGTH; i++) {
        int signal_index = start_index - i;
        if (signal_index >= 0) {
            output += filter_coefficients[i] * input_signal[signal_index];
        }
    }

    return output;
}



int main() {
#if 0    
    for(int i=0;i<N;i++) {
        const int r=-1000 + (random()%1000);
        input_samples[i]+=r;
    }
#endif    
    
    float rec[N], ref[N];
    // refbuf: 前回の再生結果(前半)
    for(int i=0;i<N;i++) ref[i]=played_samples[i];
    for(int i=0;i<N;i++) rec[i]=recorded_samples[i];

    
    printf("rec:\n");
    dump_samples_f(rec,N);
    save_f(rec,N,"rec.raw");
    printf("ref:\n");
    dump_samples_f(ref,N);    
    save_f(ref,N,"ref.raw");
    float estimated[N];    
    float canceled[N];
    float err[N];    
    for(int loop=0;loop<100;loop++) {
        printf("loop:%d\n",loop);
        // エコーを推定する
    
        for(int i=0;i<N;i++) estimated[i]=fir_filter(ref,i);
        printf("estimated:\n");
        dump_samples_f(estimated,N);
        // エコーを除去した信号をつくる
        for(int i=0;i<N;i++) canceled[i]=rec[i]-estimated[i];
        printf("canceled:\n");
        dump_samples_f(canceled,N);

        // エコーを減算する。これがエラー信号となる。エラー信号が小さければ良い。

        for(int i=0;i<N;i++) err[i]=rec[i]-estimated[i];
        float mse=calc_mse(err,N);
        printf("err: mse:%f\n",mse);
        dump_samples_f(err,N);
        if(mse<0.000001) {
            printf("found a good coefficients, quit loop\n");
            break;
        }        
        // フィルタ係数を更新
        const float u=0.2;
        for(int i=0;i<N;i++) {
            for (int j = 0; j < N; j++) {
                if (i - j >= 0) {
                    filter_coefficients[j] += u * err[i] * ref[i - j];
                }
            }
        }
        printf("filter_coefficients:\n");
        dump_samples_f(filter_coefficients,FILTER_LENGTH);

        printf("===============\n");
    }
    // 係数の更新が終わっているので、この時点でのcanceledが出力である
    printf("finally output(canceled):\n");
    dump_samples_f(canceled,N);
    save_f(canceled,N,"final.raw");
    
}
