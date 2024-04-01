#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void generate_random_walk_noise(float* output, int num_samples, float intensity) {
    srand(time(NULL));  // 乱数シードを設定

    output[0] = 0.0f;  // 最初のサンプルを0に初期化

    for (int i = 1; i < num_samples; i++) {
        float random_value = (float)rand() / (float)RAND_MAX;  // 0から1までの乱数を生成
        float step = (random_value - 0.5f) * intensity;  // ステップの大きさを計算
        output[i] = output[i - 1] + step;  // 前のサンプルにステップを加算して新しいサンプルを生成

        // 値が0から離れるほど早く0に戻るようにする
        float pull_factor = 1.0f - powf(fabs(output[i]), 0.5f);
        output[i] *= pull_factor;

        // 出力を1と-1の間にクリップ
        if (output[i] > 1.0f) {
            output[i] = 1.0f;
        } else if (output[i] < -1.0f) {
            output[i] = -1.0f;
        }
    }
}

int main() {
    float buf[100];
    generate_random_walk_noise(buf,100,0.2);
    for(int i=0;i<100;i++) {
        printf("%.3f ", buf[i]);
    }
    printf("\n");
}
