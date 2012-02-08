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

# メッセージを処理するのだ
	processmsg( $client, $port , $_ );
}

#
#
# クライアントからのメッセージを処理する
#
#
# MYADDR 自分のアドレスを教えるためのコマンド(なくてもいいのだが)
# GETOTHER 他のクライアントのアドレスを知るためのコマンド
#
# GETOTHERで他のクライアントのアドレスを知ってそのクライアントにパケッ
# トを送ってみるのだ。そこまで自動。
sub processmsg {

	$client = shift( @_ );
	$port = shift( @_ );
	$msg = shift( @_ );

	print "process: $client $port $msg \n";

	if( $msg =~ /MYADDR/ ){
		print "Address information from client $client:$port $msg\n";

# クライアントのリストに加える
		push( @caddr , $client );
		push( @cport , $port );
# リストを表示
		printClients();
		
	}
}

sub printClients
{

	
	
}
