#define N 100000
int main(int argc, char *argv[])
{
        int i, a[N];
          #pragma omp parallel for
        for (i=0;i<N;i++)
                a[i]= 2*i;
        return 0;
}
