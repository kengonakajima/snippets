use Socket;

$port = 9100;

$proto = getprotobyname('udp');


socket( S , PF_INET , SOCK_DGRAM , $proto ) || die "socket : $!";

$ent = sockaddr_in( $port , INADDR_ANY );
bind( S , $ent ) || die "bind: $!";

select( S ); $| = 1; select( STDOUT );

while( 1){
	my $from = recv( S , $_ , 1000 , 0  );
	my ( $port , $ipaddr ) = sockaddr_in( $from );
	my $client = inet_ntoa( $ipaddr );

	print "Received $_ from $client:$port " , length($from) , "\n" ;
	send( S , "your message [$_]" , 0 , $from ) || die "send : $!";

# $B%a%C%;!<%8$r=hM}$9$k$N$@(B
	processmsg( $client, $port , $_ );
}

#
#
# $B%/%i%$%"%s%H$+$i$N%a%C%;!<%8$r=hM}$9$k(B
#
#
# MYADDR $B<+J,$N%"%I%l%9$r65$($k$?$a$N%3%^%s%I(B($B$J$/$F$b$$$$$N$@$,(B)
# GETOTHER $BB>$N%/%i%$%"%s%H$N%"%I%l%9$rCN$k$?$a$N%3%^%s%I(B
#
# GETOTHER$B$GB>$N%/%i%$%"%s%H$N%"%I%l%9$rCN$C$F$=$N%/%i%$%"%s%H$K%Q%1%C(B
# $B%H$rAw$C$F$_$k$N$@!#$=$3$^$G<+F0!#(B
sub processmsg {

	$client = shift( @_ );
	$port = shift( @_ );
	$msg = shift( @_ );

	print "process: $client $port $msg \n";

	if( $msg =~ /MYADDR/ ){
		print "Address information from client $client:$port $msg\n";

# $B%/%i%$%"%s%H$N%j%9%H$K2C$($k(B
		push( @caddr , $client );
		push( @cport , $port );
# $B%j%9%H$rI=<((B
		printClients();
		
	}
}

sub printClients
{

	
	
}
