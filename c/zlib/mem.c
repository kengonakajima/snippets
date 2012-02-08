#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

char buf[1024*1024*32];
char enc[1024*1024*32];
char vbuf[1024*1024*32];

int main(int argc, char **argv)
{
    FILE *fp;
    int r;
    int tot=0;
    int i;
    
    
    
    if(argc <2){
        printf("fuck\n");
        return 1;
    }

    for(i=1;;i++){
        if( !argv[i])break;
        
        fp=fopen(argv[i],"r");
        r = fread( buf, 1, sizeof(buf),fp);
        fclose(fp);


        r = z_mem_compress( enc, sizeof(enc), buf, r );
        printf("compress r=%d\n",r);
        tot += r;
        r = z_mem_uncompress( vbuf, sizeof(vbuf), enc, r );
        printf( "uncompress r=%d\n",r);

        if( memcmp( buf, vbuf, r ) != 0){
            printf( "DIFFER\n");
        }
    }
    printf( "TOT=%d\n", tot );
    return 0;
}
int z_mem_uncompress(
                    char *out, int outlen,
                    char *in, int inlen )
{
    char buf[4096];
    int out_so_far =0;
    z_stream z;
    
    memset(&z,0,sizeof(z));
    z.zalloc = NULL;
    z.zfree = NULL;
    z.opaque = NULL;
    if(inflateInit( &z ) != Z_OK ){
        return -123;
    }
    
    z.next_in = (void*)in;
    z.avail_in = inlen;
    z.next_out = buf;
    z.avail_out = sizeof(buf);

    while(1){
        int r,outsz;
//        printf( "AAAAAA %d\n",out_so_far );
        r = inflate( &z, Z_NO_FLUSH );
        if( r == Z_STREAM_END || r == Z_BUF_ERROR ){
            break;
        }
        if( r != Z_OK) {
            printf( "fuck8 %d\n", r);
            return -1234;
        }

        /* inflation OK! */
        outsz = sizeof(buf)- z.avail_out;
        if( (out_so_far +outsz)> outlen){
            printf( "fuck9\n" );
            return -12345;
        }
        memcpy( out + out_so_far , buf, outsz );
        out_so_far += outsz;
        z.next_out = buf;
        z.avail_out = sizeof(buf);
    }

    return out_so_far;
}

int z_mem_compress( 
                    char *out, int outlen,
                    char *in, int inlen )
{
    char buf[4096];
    int out_so_far = 0;
    z_stream z;
    
    memset(&z,0,sizeof(z));
    z.zalloc = NULL;
    z.zfree = NULL;
    z.opaque = NULL;
    if(deflateInit( &z, Z_DEFAULT_COMPRESSION ) != Z_OK ){
        return -123;
    }
    
    z.avail_in = inlen;
    z.next_in = in;

    /* got hint from ssh3 code */
    do {

        int r,outsz;
        
        z.next_out = buf;
        z.avail_out = sizeof(buf);
        if( z.avail_in != 0 ){
            r = deflate( &z, Z_PARTIAL_FLUSH);
        } else {
            r = deflate( &z, Z_SYNC_FLUSH );
        }

        if( r == Z_OK ){
            outsz = sizeof(buf) - z.avail_out;
            if( (out_so_far + outsz) > outlen ){
                printf( "fuck4\n");
                deflateEnd(&z);
                return -1234;
            }
            memcpy( out + out_so_far, buf, outsz );
            out_so_far += outsz;
        } else {
            printf( "fuck3 : %d\n", r );
            deflateEnd(&z);
            return -12345;
        }
    } while( z.avail_out == 0 );

    deflateEnd(&z);
    return out_so_far;
}


