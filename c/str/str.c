#define _STR_C_

#include <string.h>
#include <ctype.h>


#define ON 	1
#define OFF 0



main()
{
	char line[]="IMAGE fuck type=fucker mode=hoge baka=aho\n" ;

	char token[1000];

	getEscapedTokenByTop( line , "type=" , token, sizeof(token ));
						 
	printf( "%s\n"  ,  token );

}


/***************************
  ��ʸ������ʸ������̤��ʤ�ʸ�������
  Ʊ�����ɤ�������Ĵ�٤ʤ���
  
***************************/
int strcmpIcase(char *s1, char *s2 )
{

	while( *s1 != '\0' && *s2 != '\0' ){
		if( tolower(*s1++) != tolower( *s2++ )){
			return ON;
		}
	}

	return 0;
}

/***************************
  ʸ����򥨥������פ��롣���������פ����٤�����ϡ�

  0x20 ���ڡ���
  0x3d =
  0x5c \
  0x22 "
  0x28 (
  0x29 )
  0x5b [
  0x5d ]
  
***************************/
void escapeString( char *src , char *dest )
{
	int sc=0,dc=0;

	for(sc=0,dc=0; ;sc++ ){
		if( src[sc]== ' ' || src[sc]== '=' ||
		   src[sc]=='\\' || src[sc]=='\"' || src[sc]=='(' || src[sc]==')' ||
		   src[sc]=='[' || src[sc]==']' || src[sc] == ',' ){
			dest[dc++] = '\\';
			dest[dc++] = src[sc];
		} else {
			dest[dc++] =src[sc];
		}
		if( src[sc] == '\0' )break;
	}
}


/***************
  ���������ץ���  ���󥹤��б�����ʸ��������ȡ�

  char * buf ... ʸ����
  int escc ..... ���������׵���
  int c ........ ����������ʸ��
  int n ........ ������ȿ�
  
***************/
int countCharEscaped( char *buf , int escc , int c ,int n)
{
    int i;
    int count = 0;
    for(i = 0 ; ;i++){
		if( buf[i] == '\0' ) break;
		if( buf[i] == c && i > 0 && buf[i-1] != escc ){
			count++;
			if( count == n ) return i;
		}
    }

    return -1;
}


/******************
  ���������׵�����б������ȡ����ʥ���



  ���������׵���ϡ� \�ǡ����켫�Τ򤢤�魯�ˤϡ�\\�Ȥ��롣

  ʸ��c�Ƕ��ڤ�줿count���ܤΥȡ������dest�˥��ԡ����롣0���ܤ���Ϥ��ޤ롣
  dest�Ϥ��Ǥ˳��ݤ���Ƥ����ΰ��ؤ��ݥ��󥿡�
  return value .... 0  ̵���˥ȡ����󤬤ߤĤ��ä���
                    <0 Error


  maxlen�ޤǤ�Ĺ����dest�˽��Ϥ���롣
  
******************/
int getEscapedToken( char *src , int c , int count , char *dest , int maxlen)
{
	int en=0,sn=0;
	int i=0,ii=0;

	if( count == 0 ){
		sn = 0;
		en = countCharEscaped( src , '\\' , c , 1 );
	} else if( count > 0 ){
		sn = countCharEscaped( src , '\\' , c , count );
		en = countCharEscaped( src , '\\' , c , count+1 );
		if( en < 0 ) en = strlen( src );
		if( sn < 0 ){
			return -1;
		}
		sn++;
	} else {
		return -2;
	}
	ii=0;
	for(i=sn;i<en ; i++ ){
		
		if( src[i] == '\\' ){
			/* Escaped character */
			dest[ii++] = src[i+1];
			i++;
		} else {
			/* NonEscaped character */
			dest[ii++] = src[i];
		}
		if( ii >= (maxlen -1))break;
	}
	dest[ii] = '\0';
	return 0;
}

/***********************
  ���ꤵ�줿��Ƭ��ʸ�������ĥȡ�������֤����ȡ�����ϥ��������פ���Ƥ���
  ����Ǥ�����줿��ΤȤ��롣
  �ȡ����󤬤ʤ��ä����ϡ�����ʸ������֤���

***********************/
void getEscapedTokenByTop( char *src , char *type , char *output ,int outputlen)
{
	int i,l;

	output[0] = '\0';
	l = strlen( type );

	
	for(i=0;;i++ ){
		output[0] = '\0';
		if( getEscapedToken( src , ' ', i , output , outputlen) < 0 ){
			break;
		}
		if( strncmp( output , type , l ) == 0 ){
			for(i=0;i<outputlen;i++){
				output[i] = output[i+l];
				if( output[i] == '\0'){
					break;
				}
			}
			output[outputlen-1]='\0';		/* �����˽���餻�� */
			return;
		}
	}
}

/*************************
  ʸ������������ʸ�����֤������롣
*************************/
void replaceChar( char *buf , int from , int  to)
{
	
	while( *buf ){
		if( *buf == from ) *buf = to;
		buf++;
	}
}

/*************************
  �ե�����̾�������γ�ĥ�Ҥ����Τ�Τ��Ѥ���  
  *************************/
void changeFileNameTail( char *name ,char *tail)
{
    int len;
    int i;
    int pos =0;
	
    len = strlen( name );

    /* ������롣 */
	
    for(i = 0 ; i < len ; i++){
		if( name[len-i] == '.' ){
			pos = len-i;
			name[len-i+1] = '\0';
			break;
		}
    }
    if( pos==0)return;

    strcat( name , tail );
}

/*************************
  ��ʸ�����ʸ�������ˤ���
  *************************/
void lowerCase( char *src )
{
    int c;
	
    while( (c = *src) != '\0' )
    {
		if( c >= 'A' && c <= 'Z' ) *src = c - ('a'-'A');
		src++;
    }
}

/*************************
  ���Ե������ʸ���󤫤������Ƶͤ�롣
  
  *************************/
void removeCRandLF( char *src )
{
    int i;
    for(i = 0 ; ; i++){
		if( src[i] == '\0')break;
		if( src[i] == 10 || src[i] == 13 ){
			shiftCharArray( src , i );
			i--;
		}
    }
}

/*************************
  ʸ�����nʸ���ܤ�������Ƶͤ�롣
  ����ʸ����ϥ��ǥ��åȤ���롣
  �ͤ��ϰϤʤɤϡ���̥롼����ˤ�Ǥ����
  *************************/
void shiftCharArray( char *src , int n )
{
    int i;
    for(i = n ; ;i++){
		if((src[i] = src[i+1])==0)return;
    }
}

/*************************
  byteArrayLineInputStream�Τ褦�ʤ��Ȥ򤹤롣ʸ���󤬤��ä��顢����ʸ���󤫤�
  ��Լ������ơ��ͤ�롣����ʸ������Ѳ�����Τ����
  *************************/
void cutLineFromCharArray( char *src , char *dest )
{

    int i;
    int l;
	
    for(i  = 0 ; ;i++){
		
		dest[i] = src[i];
		if( src[i] == '\n' ) break;
		if( src[i] == '\0' ) break;
    }
    dest[i+1] = '\0';

    l = strlen( dest );
    for(i = 0 ;  ;i++){
		src[i] = src[i+l];
		if( src[i+l] == '\0')break;
    }	
}
/*************************
  ʸ���󤬿����󤫤ɤ���Ĵ�٤롣
  *************************/
int isnumstr(char *s )
{
    int c;

    if( s[0] == '-' ) s++;
	
    while( ( c = *s++) != '\0' ) if(  ! isdigit( c )) return 0;

    /* ����ֿ������ä� */
    return 1;
	
}

/****************************************************
  ���ѤΥ�����
  ****************************************************/

/*****
  ���ꤵ�줿ʸ�������ꤵ�줿����ФƤ����顢���ΰ��֤��֤���
  ���֤����顢ʸ�����0���ܤ˽ФƤ�����0���֤뤾��
  ���ꤵ�줿�Ŀ����ʤ��ä��顢-1���֤���
  *****/   
int countChar( char *buf , int c ,int n)
{
    int i;
    int count = 0;
    for(i = 0 ; ;i++){
		if( buf[i] == '\0' ) break;
		if( buf[i] == c ){
			count++;
			if( count == n ) return i;
		}
    }

    return -1;
}




/**************************
  ʸ����β��Ե���򻦤���ʸ�����1�ԤΥǡ����Ȥ��롣
  **************************/
char * chop( char *buf )
{
    char *backup = buf;
    while( *buf++){
		if( *buf == 10 || *buf == 13 ){
			*buf='\0';
			break;
		}
			
    }
    return backup;
}

