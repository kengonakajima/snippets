#
# $B$G!<$?$r$b$8$K$9$k$N(B
# perl$BMQ(B
#

use strict subs;


# print remotestrcapcount( "This is a SAMPLE string.\n" , 10 ) ,"\n";
print remotesum2( "answer",( 10 , 20 , 30 , 44 , 5 , 60 ,77,88,90,100) );



#
# $B0J2<$N=hM}$O%f!<%6Dj5A(B
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
# $B%@%_!<$J%H%i%s%9%]!<%H(B
#
sub dummy_tcp_subsystem($)
{
	return dispatcher( shift );
}
