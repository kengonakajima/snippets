#include <math.h>
#include <stdio.h>

#define PI 3.141592653

int main()
{
    int d = 100;
    int l = 8;
    float rad = d * 2 * PI / 360.0f;

    printf("%d,%d\n", (int)(l * cos(rad)) , (int)(l * sin(rad)) );

    return 0;
}
