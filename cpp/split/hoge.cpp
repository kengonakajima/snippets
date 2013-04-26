#include <stdio.h>
#include <string.h>

void getToken( const char *s, int ind, char delim, char *out, int *outlen  ) {
    int tki=0;
    int outi=0;
    int l = strlen(s);
    for(int i=0;i<l;i++){
        char c = s[i];
        //        printf("i:%d c:%c tki:%d outi:%d\n", i, c, tki, outi );
        if( c == delim ) {
            tki ++;
            if( tki > ind ) break;
            continue;
        }
        if( tki == ind ) {
            out[outi] = c;
            outi++;
            if( outi == *outlen-1) break;
        }
    }
    out[outi] = '\0';
    *outlen = outi;
}
int main() {
    const char url[] = "GET /poo HTTP/1.0\r\n";

    char p[30];
    int len;

    printf( "url:%s\n",url );

    for(int i=0;i<4;i++) {
        len = sizeof(p);
        getToken( url, i, ' ', p, &len );    
        printf( "%d : %s len:%d\n", i, p, len );
    }
}
