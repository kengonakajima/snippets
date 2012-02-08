#include <stdio.h>     // printf使用
#include <stdlib.h>     // qsort使用


// 比較関数 : 二つの int型の値を比較する
int compare(const void *value1, const void *value2)
{
    // 結果により、正の値、0、負の値のいずれかを返す
    return *(const int *)value1 - *(const int *)value2;
}

// main 関数
int main() 
{
    int a[] = {7, 3, 9, 1, 5, 2, 6, 0, 8, 4};    // 並べ替えるデータ
 
    qsort(a, 10, sizeof(int), compare);    // クイックソート

    int i;
    for (i=0; i<10; i++) {
        printf("%d, ", a[i]);    // 出力結果　[ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,]
    }


} 
