#include <stdio.h>
enum class Hoge : int {
                       BABA = 0,
                       KEKE = 1,
};
int a[2];
int main() {
    a[(int)Hoge::BABA]=100;
    a[(int)Hoge::KEKE]=222;
    printf("%d,%d\n",a[0],a[1]);
}
