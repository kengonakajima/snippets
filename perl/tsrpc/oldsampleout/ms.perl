#
# でーたをもじにするの
# perl用
#

use strict subs;


# print remotestrcapcount( "This is a SAMPLE string.\n" , 10 ) ,"\n";
print remotesum2( "answer",( 10 , 20 , 30 , 44 , 5 , 60 ,77,88,90,100) );



#
# 以下の処理はユーザ定義
#
# ( str , int32[10] )
sub remotesum2_TSRPC(@)
{								
	$fuck= shift;
	print "FUCK is: $fuck\n";
	my $a=0 , $out=0;
	for $a ( @_ ){
		print "A is : $a\n";
		$out += $a;
	}
	return $out;
}









#
# ダミーなトランスポート
#
sub dummy_tcp_subsystem($)
{
	return dispatcher( shift );
}
