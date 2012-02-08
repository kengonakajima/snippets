#include <stdio.h>

int main()
{
    char data[1024];
    FILE *fp = fopen( "aho.txt","r");
    fread(data,1,sizeof(data),fp);
    fclose(fp);

    int i;
    for(i=0;;i++){
        if( data[i]==0 )break;
        printf("%x\n", data[i]);
    }

    char hoge[] = { 0xe3, 0x81, 0xbb, 0xe3, 0x81, 0x92, 0x0a };
    fp = fopen( "hoge.txt","w");
    fwrite( hoge, 1, sizeof(hoge), fp);
    fclose(fp);
}


/*
ほげ
日本語
abc0123
終わり

という内容のファイルを od -cxすると、

0000000   ほ  **  **  げ  **  **  \n  日  **  **  本  **  **  語  **  **
             81e3    e3bb    9281    e60a    a597    9ce6    e8ac    9eaa
0000020   \n   a   b   c   0   1   2   3  \n  終  **  **  わ  **  **  り
             610a    6362    3130    3332    e70a    82b5    82e3    e38f
0000040   **  **  \n                                                    
             8a82    000a                                                
0000043

となる、プログラムを実行すると、　以下のようになる。


ffffffe3
ffffff81
ffffffbb
ffffffe3
ffffff81
ffffff92
a
ffffffe6
ffffff97
ffffffa5
ffffffe6
ffffff9c
ffffffac
ffffffe8
ffffffaa
ffffff9e
a
61
62
63
30
31
32
33
a
ffffffe7
ffffffb5
ffffff82
ffffffe3
ffffff82
ffffff8f
ffffffe3
ffffff82
ffffff8a
a


 */
