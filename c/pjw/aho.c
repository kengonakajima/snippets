

/* ハヤくてけっこういいはっしゅ */
unsigned int hashpjw ( char* s )
{
    char *p;
    unsigned int h= 0 ,g;
    for( p = s ; *p ; p ++ ){
        h = ( h<< 4 ) + (*p);
        if( (g = h & 0xf0000000) != 0){
            h = h ^ (g>>24);
            h = h ^ g;
        }
    }
    return h;
}


void makename( char *name )
{
    char c[] = { '0','1','2','3','4','5','6','7','8','9',
                 'a','b','c','d','e','f','g','h','i','j',
                 'k','l','m','n','o','p','q','r','s','t',
                 'u','v','w','x','y','z' };
    int i;
    char out[100];
    for(i=0;i<13;i++){
        out[i] = c[ rand() % sizeof(c)];
    }
    out[i] = 0;
    sprintf( name, "%s%%m1.toybox.net", out );
}

int main()
{
    int i;
    int count[256];
    memset(count,0,sizeof(count));
    
    for(i=0;i<1000000;i++){
        char name[1000];
        int h;
        int mod;
        
        makename(name);
        h = hashpjw(name);
        mod = (h>>4) % 256;
        count[mod]++;
    }
    for(i=0;i<256;i++){
        printf( "%5d %9d   | ", i, count[i] );
        if( (i%4)==3){
            printf("\n");
        }
    }
}
