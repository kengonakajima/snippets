use Socket;

$host = "lsdev.lan.titan.co.jp";


my $proto = getprotobyname( 'udp' );
socket( S , PF_INET , SOCK_DGRAM , $proto );

$ent = sockaddr_in( $port = 9100  , inet_aton( $host ) );

select( S ); $| = 1; select( STDOUT );

print "HOST: $host PORT: $port \n";

while( 1 )
{
	print "input a message:" ;

	chomp( $_ = <STDIN> );
	print "sending $_\n";
	send( S , $_ , 0 , $ent );
	recv( S , $_, 1000 , 0 );
	print "responce is : $_\n" ;
	
}
