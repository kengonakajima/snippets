

int makeTmpFileName( char *orig , char *tmpbase , char *output , int len )
{
    /* / ��������ϡ����餫�Υǥ��쥯�ȥ����򤷤Ƥ��롣
       �ʤ����ϡ��ե�����̾��ľ�ܻ��ꤷ�Ƥ���������� */

    int i;
    int slash_ind = -1;
    
    for(i=0;;i++){
        if( orig[i] == '/' ) slash_ind = i;
        if( orig[i] == '\0' ) break;
    }

    if( slash_ind == -1 ){
        /* ����ä��夬�ʤ����.output�ˤ��Τޤޤ��ԡ� */
        strncpy( output , tmpbase, len-1 );
        output[len-1]='\0';     /* �⤸��Ľ���餻�� */
    } else {
        /* ����å��夬���ä����Ǹ�Υ���å���ΰ��֤�
           slash_ind �ˤϤ��äƤ�Τǡ�����ʸ���ޤǤ� 
           strncpy����Ȥ褤 */
        if( ( strlen( orig ) + strlen( tmpbase )
        strncpy( output , orig , slash_ind + 1 );
        output[slash_ind + 2] = '\0';
    }
    
    
    
}

int main()
{
    
}

