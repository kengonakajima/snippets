

/*
  MySQL と同じロジックの escaper
  出力先は，fromlen*2+1 ないといけない(エラー)．
  データ壊さずにエラー返す
 */
int toybox_escape_string( char *to, int tolen, char *from, int fromlen )
{
  const char *to_start=to;
  const char *end;

  if( tolen < (fromlen*2+1)) return -1;
  
  for (end=from+fromlen; from != end ; from++)
  {
    switch (*from) {
    case 0:				/* Must be escaped for 'mysql' */
      *to++= '\\';
      *to++= '0';
      break;
    case '\n':				/* Must be escaped for logs */
      *to++= '\\';
      *to++= 'n';
      break;
    case '\r':
      *to++= '\\';
      *to++= 'r';
      break;
    case '\\':
      *to++= '\\';
      *to++= '\\';
      break;
    case '\'':
      *to++= '\\';
      *to++= '\'';
      break;
    case '"':				/* Better safe than sorry */
      *to++= '\\';
      *to++= '"';
      break;
    case '\032':			/* This gives problems on Win32 */
      *to++= '\\';
      *to++= 'Z';
      break;
    default:
      *to++= *from;
    }
  }
  *to=0;
  return (to-to_start);
}
/* 出力先が足りない場合，壊すよ.fromと同じだけとってたらいいけどね */
int toybox_descape_string( char *to, int tolen, char *from )
{
    int i,so_far;
    int l = strlen(from);

    so_far = 0;
    for(i=0;i<l;i++){
        if( from[i] == '\\' ){
            switch( from[i+1] ){
            case '0':
                to[so_far] = '\0';
                break;
            case 'n':
                to[so_far] = '\n';
                break;
            case 'r':
                to[so_far] = '\r';
                break;
            case '\\':
                to[so_far] = '\\';
                break;
            case '\'':
                to[so_far] = '\'';
                break;
            case '\"':
                to[so_far] = '\"';
                break;
            case 'Z':
                to[so_far] = '\032';
                break;
            }
            i++;
            so_far++;
        } else {
            to[so_far] = from[i];
            so_far++;
        }
        if( so_far ==(tolen-1)){
            break;
        }
    }
    to[so_far]='\0';
}


int main()
{
    char aho[] = "日本語でも何のモンダイもなし";
    char hoge[] = "asdfasdf\nasdfasdfa\n\"\"\"\"";
    char nasu[] = "aksdjfalksjdflkajsdf\nasdkfalsdkfjalksjdflakjdslkjsdflkajdsfaalksdjflakjsdflkajsdlfkjahsdlkfhjhaksjaksdfhljsdhajdfladahlkadkjdfasdfn\\\n\n";
        

    char out[1000], dout[1000];
    toybox_escape_string( out, sizeof(out), aho, strlen(aho) );
    printf( "%s\n", out );
    toybox_descape_string( dout,sizeof(dout), out);
    printf( "%s\n", dout );
    
    toybox_escape_string( out, sizeof(out), hoge, strlen(hoge) );
    printf( "%s\n", out );
    toybox_descape_string( dout,sizeof(dout), out);
    printf( "%s\n", dout );
    
    toybox_escape_string( out, sizeof(out), nasu, strlen(nasu) );
    printf( "%s\n", out );
    toybox_descape_string( dout,sizeof(dout), out);
    printf( "%s\n", dout );
    
}
