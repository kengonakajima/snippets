int main()
{

    int i;
    int bits = 1024;
    int short_bits = 16;
    int loop = bits / short_bits;
    short result[loop];

    goto aho;
    for(i=0;i<loop;i++){
        result[i]=rand() / 64;
        printf( "%04x", result[i] & 0xffff );
    }
 aho:
    {
        int i = 0xfe80;
        short s = i;

        printf( "%x\n",s & 0xffff );
    }
}
