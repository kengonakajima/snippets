#include <stdio.h>
enum class Hoge : int {
                       BABA = 0,
                       KEKE = 1,
                       PUKA = 2,
                       HEGO = 4,
};
int a[2];
int main() {
    a[(int)Hoge::BABA]=100;
    a[(int)Hoge::KEKE]=222;
    printf("%d,%d or:%d\n",a[0],a[1], (int)Hoge::PUKA | (int)Hoge::HEGO);
}
