

int makeTmpFileName( char *orig , char *tmpbase , char *output , int len )
{
    /* / がある場合は、何らかのディレクトリ指定をしている。
       ない場合は、ファイル名を直接指定しているだけだ。 */

    int i;
    int slash_ind = -1;
    
    for(i=0;;i++){
        if( orig[i] == '/' ) slash_ind = i;
        if( orig[i] == '\0' ) break;
    }

    if( slash_ind == -1 ){
        /* すらっしゅがないよん.outputにそのままこぴー */
        strncpy( output , tmpbase, len-1 );
        output[len-1]='\0';     /* もじれつ終わらせる */
    } else {
        /* スラッシュがあった。最後のスラッシュの位置は
           slash_ind にはいってるので、その文字までを 
           strncpyするとよい */
        if( ( strlen( orig ) + strlen( tmpbase )
        strncpy( output , orig , slash_ind + 1 );
        output[slash_ind + 2] = '\0';
    }
    
    
    
}

int main()
{
    
}

