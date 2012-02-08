
struct AA
{
	int ai;
	char aa[100];
};

typedef double* dp;


struct BB
{
	dp bd;
	int bi;
	struct AA ba;
	struct AA *bap;
};
typedef struct CC
{
	struct BB cb;
} CCtype;


struct AA aaa;
void AAEncoder( struct AA *lpAA , char *super );
void BBEncoder( struct BB *lpBB, char* super );
void CCEncoder( struct CC *lpCC , char*super  );
void strcpyEx( char *dest, char*src );
void AADecoder( struct AA *lpAA , char *super  , char *data );
void BBDecoder( struct BB *lpBB , char *super , char *data );

char* descapeString( char*a );
char*  escapeString( char*a );
main()
{
	struct BB b;
	struct CC c;
	struct AA a;
	
	strcpy( a.aa, "Fuck.");
	
	b.bd = (dp)0;
	b.ba.ai = 2004;
	b.bap = &aaa;
	b.bap->ai = 3090;
	strcpy( b.ba.aa , "This is a test.");
	BBEncoder( &b , "b" );

	c.cb = b;

	CCEncoder( &c , "c" );

	AADecoder( &a , "a" , "a.ai=100\na.aa=To\\SBe\\SDecoded\na.hoge=DummyData\n" );
	printf( "a.ai=%d a.aa=%s\n" , a.ai , a.aa );

	BBDecoder( &b , "b" , "b.bd=0\nb.bi=134511412\nb.ba.ai=2004\nb.ba.aa=This\\Sis\\Sa\\Stest.\nb.bap=804a38c\n" );
	printf( "b.bd=%d b.bi=%d b.ba.aa=%s b.ba.ai=%d\n",
		   b.bd, b.bi , b.ba.aa , b.ba.ai );
	
}
char tmpstr[1024];
char escapework[1024];


void AADecoder( struct AA *lpAA , char *super ,char *data )
{
	int l,i;
	for(i=0;data[i] !='\0';i++){
		if( i == 0 || data[i-1] == '\n' ){
			sprintf( tmpstr,  "%s.ai=" , super );
			if( strncmp( tmpstr , data + i , l = strlen( tmpstr )) == 0){
				lpAA->ai = strtol( data + i + l , (char*)0 , 10 );
			}
			sprintf( tmpstr , "%s.aa=" , super );
			if( strncmp( tmpstr , data + i , l = strlen( tmpstr )) == 0 ){
				strcpyEx( tmpstr , data + i + l );
				strcpy( lpAA->aa , descapeString( tmpstr ) );
			}
		}
	}
}
void BBDecoder( struct BB *lpBB , char *super , char *data )
{
	int l,i;
	for(i=0;data[i] !='\0';i++){
		if( i == 0 || data[i-1] == '\n' ){
			sprintf( tmpstr,  "%s.bd=" , super );
			if( strncmp( tmpstr , data + i , l = strlen( tmpstr )) == 0){
				lpBB->bd = (dp)strtol( data + i + l , (char*)0 , 16 );
			}
			sprintf( tmpstr , "%s.bi=" , super );
			if( strncmp( tmpstr , data + i , l = strlen( tmpstr )) == 0 ){
				lpBB->bi = strtol( data +i + l  ,(char*)0,10);
			}
			sprintf( tmpstr , "%s.ba" , super );
			AADecoder( &( lpBB->ba ) , tmpstr , data );
			sprintf( tmpstr , "%s.bap=" , super );
			if( strncmp( tmpstr , data + i , l = strlen( tmpstr )) == 0 ){
				lpBB->bap = (struct AA*)strtol( data +i + l  ,(char*)0,16);
			}
		}
	}	
}


void CCEncoder( struct CC *lpCC , char*super )
{
	sprintf( tmpstr , "%s.cb" , super );
	BBEncoder( &( lpCC->cb) , tmpstr );
}
void BBEncoder( struct BB *lpBB, char* super )
{

	printf( "%s.bd=%x\n" , super , lpBB->bd );
	printf( "%s.bi=%d\n" , super , lpBB->bi );
	sprintf( tmpstr , "%s.ba" , super );
	AAEncoder( &( lpBB->ba ) , tmpstr  );
	printf( "%s.bap=%x\n" , super , lpBB->bap );
}
void AAEncoder( struct AA *lpAA , char *super )
{
	printf( "%s.ai=%d\n" , super , lpAA->ai );
	printf( "%s.aa=%s\n" , super , escapeString(lpAA->aa) );
}


/* give up copying when finding '\0' or '\n' */
void strcpyEx( char *dest, char*src )
{
	while(*src){
		*dest = *src;
		if( *src == '\0' || *src == '\n' )break;
		*dest++;
		*src++;
	}
}





/*********************
	Escape a string
	Escape only  ' ' , '\\', '\n'
*********************/
char*  escapeString( char*a )
{
	int i,c=0;

	escapework[0] = '\0';

	for(i=0;;i++){
		if( a[i] == '\0' ){
			escapework[c++] = '\0';
			break;
		} else if( a[i] == '\\' ){
			escapework[c++] = '\\';
			escapework[c++] = '\\';
		} else if( a[i] == ' ' ){
			escapework[c++] = '\\';
			escapework[c++] = 'S';
		} else if( a[i] == '\n' ){
			escapework[c++] = '\\';
			escapework[c++] = 'n';
		} else {
			escapework[c++] = a[i];
		}
	}

	return escapework;
}
char* descapeString( char*a )
{
	int i ,c =0;

	escapework[0] = '\0';

	for(i=0;;i++){
		if( a[i] == '\0' ){
			escapework[c++]='\0';
			break;
		} else if( a[i] == '\\' ){
			if( a[i+1] == 'S' ){     /* space */
				escapework[c++] = ' ';
			} else if( a[i+1] == 'n' ){
				escapework[c++] = '\n';
			} else if( a[i+1] == '\\' ){
				escapework[c++] = '\\';
			} else {
				escapework[c++] = a[i];
			}
			i++;           
		} else {
			escapework[c++] = a[i];
		}
	}
	return escapework;
}
