#
#
#  TCP select server
#
$| =1;

use IO::Socket;
use IO::Select;

# 普通にソケット作る
$mainsock = new IO::Socket::INET(
							 LocalHost => "localhost" , 
							 LocalPort => 10100,
							 Proto => 'udp' , 
								 );

die "FUCK!" unless $mainsock;

print $mainsock , "\n" ;


$rfdset = new IO::Select();
$rfdset->add( $mainsock );

while(1)
{
	($new_readable ) = IO::Select->select( $rfdset , undef , undef ,0 );
	foreach $sock ( @$new_readable )
	{
		$hoge = "ahoahoahoha";
		$adr = $mainsock->recv($hoge , 100 );		

		($port, $a )= sockaddr_in( $adr );
		$host = inet_ntoa( $a );

		print "adr: <" , $host, " "  , $port , ">\n";
		print "msg: <$hoge>\n";
	}

}






close( $mainsock );
