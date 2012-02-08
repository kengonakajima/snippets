#include <stdio.h>

/*
  引用した文字列を生成する．
  出力バッファが足りない場合は，腐らせる．

  char *insert : ">" みたいな文字列．マルチバイトでもいけるように，
  文字列にしている．
  
 */
void toybox_make_ref_text( char *out, int outlen, char *src, char *insert )
{
    int i;
    int l = strlen(src);
    int so_far;

    /* 先頭行はかならず1個 */
    snprintf( out, outlen, "%s", insert );
    so_far = strlen(out);
    for(i=0;i<l;i++){
        if( src[i]=='\n'){
            int il = strlen(insert);
            if( (so_far + il+1) >= outlen ){
                out[so_far]='\0';
                break;
            } else {
                out[so_far] = '\n';
                so_far++;
                memcpy( out + so_far, insert, il);
                so_far += il;
                out[so_far]='\0';
            }
        } else {
            if( (so_far + 1) >= outlen ){
                out[so_far]='\0';
                break;
            } else {
                out[so_far]=src[i];
                so_far++;
                out[so_far]='\0';
            }
        }
    }
}

int main()
{
    char hoge[] = "こにににちははは";
    char moge[] =
        "あほあほあほあほあふぁふぁか\n"
        "adfajakakdjhflkajsdflkjasdflkjlaksdajhkldfsljksdflahajhkjldfhklkdflajhlkadlfjasdakslafdjflakjhdflkajskjdlahkfflkjdfasdfljasdfa";

    char out[100];

    toybox_make_ref_text( out,sizeof(out), hoge, ">>" );
    printf( "HOGE: '%s'\n", out );
    
    toybox_make_ref_text( out,sizeof(out), moge, ">>" );
    printf( "HOGE: '%s'\n", out );
        
}

    
