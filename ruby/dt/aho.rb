
# "2002-02-01 00:03:04" �ߤ�����ʸ����Ʊ�Τ�
# ����Υ��Ƥ��뤫���Τ�
#
def time_diff_str(beg,fin)
    
    /(\d\d\d\d)-(\d\d)-(\d\d)\s(\d\d):(\d\d):(\d\d)/ =~ beg
    begt = Time.mktime( $1,$2,$3,$4,$5,$6 )
	
    /(\d\d\d\d)-(\d\d)-(\d\d)\s(\d\d):(\d\d):(\d\d)/ =~ fin
    fint = Time.mktime( $1,$2,$3,$4,$5,$6 )

    return ( fint - begt ).to_i
end




print time_diff_str( "2000-08-19 16:00:00" , "2002-08-19 16:00:00" ) , "\n"

