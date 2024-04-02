#include <stdio.h>

// 24KHzを12KHzにダウンサンプリングして、最大100msの遅延に対応したい場合、
// 12000 * 0.1 = 1200
// 
#define FILTER_LENGTH 5

// FIRフィルタの係数
float filter_coefficients[FILTER_LENGTH];

// 入力信号のバッファ
float input_buffer[FILTER_LENGTH] = {0};

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


void init() {
    for(int i=0;i<FILTER_LENGTH;i++) filter_coefficients[i]=0.2;
    //filter_coefficients[FILTER_LENGTH-1]=0.2;
}
int main() {
    init();

    const int N=30;
    float input_signal[N];
    for(int i=0;i<N/2;i++) input_signal[i]=0;
    for(int i=N/2;i<N;i++) input_signal[i]=0;
    input_signal[N/2]=1;

    printf("Input Signal: ");
    for (int i = 0; i < N; i++) {
        printf("%.2f ", input_signal[i]);
    }
    printf("\n");

    printf("Filtered Signal: ");
    for (int i = 0; i < N; i++) {
        float filtered_value = fir_filter(input_signal,i);
        printf("%.2f ", filtered_value);
    }
    printf("\n");

    return 0;
}
