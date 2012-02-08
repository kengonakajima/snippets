main()
{
    int i;

    for(i=0;i<256;i++){
        char b[8+1];
        if( (i & 1) == 1 ) b[7] = '1'; else b[7] = '0';
        if( (i & 2) == 2 ) b[6] = '1'; else b[6] = '0';
        if( (i & 4) == 4 ) b[5] = '1'; else b[5] = '0';
        if( (i & 8) == 8 ) b[4] = '1'; else b[4] = '0';
        if( (i & 16) == 16 ) b[3] = '1'; else b[3] = '0';        
        if( (i & 32) == 32 ) b[2] = '1'; else b[2] = '0'; 
        if( (i & 64) == 64 ) b[1] = '1'; else b[1] = '0'; 
        if( (i & 128) == 128 ) b[0] = '1'; else b[0] = '0';
        b[8] = 0;

        printf( "#define B%s %d\n" , b,i);
    }
}
