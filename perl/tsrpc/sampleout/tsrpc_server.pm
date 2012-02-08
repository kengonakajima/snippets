#
#
#
#
package tsrpc_server;

use strict subs;
use tsrpc;
use Symbol;

# 
# (  host , port ) | ( port )inaddr_any
#
sub new 
{
	my $type;
	my $host;
	my $port;

	if( @_ == 3 ){
		$type = shift;

		$host = shift;
		$port = shift;
		
	} elsif( @_ == 2 ){
		$type = shift;

		$host = undef;
		$port = shift;
	} else {
		die "usage :( host , port) / (port ) \n";
	}


	my $class = ref( $type ) || $type || "tsrpc_server" ;
	my $fh = gensym();
	my $me = bless $fh, $class;

	${*$fh}{"host"} = $host;
    ${*$fh}{"port"} = $port;


	return $me;
	
}

sub start
{	

	my $fh = shift;

	if( ${*$fh}{"host"} == undef ){
		$fh->mainloop( undef , ${*$fh}{"port"} , \&dispatcher);
	} else {
		$fh->mainloop( ${*$fh}{"host"} , ${*$fh}{"port"} , \&dispatcher);
	}

}





sub dispatcher($)
{
	$_ = shift;
	my @args = split( ' ' , $_ );
	my $funcname = shift( @args );
	my $out;

	if( $funcname eq "remotesum2" ){
		$out = tsrpc::mkstr_int32( main::remotesum2( tsrpc::demkstr_string( splice(@args,0,1)) ,tsrpc::demkstr_int32_array( splice( @args,0,10))));
	} elsif( $funcname eq "udpcat" ){
		$out = tsrpc::mkstr_string( main::udpcat( tsrpc::demkstr_string( splice(@args , 0 , 1 ) ) , tsrpc::demkstr_string( splice( @args , 0 , 1 ) ) ) );
	} else {
		$out = tsrpc::mkstr_string( "no_such_service" );
	}

	return $out;
}



#############################
# server fixed code
#
#
# ( $hostname , $port , \&dispatcher )  or
# (  $port , \&dispatcher )
#############################
sub mainloop($$$$)
{
	print join( '/' ,@_ ), "\n";
	my $fh = shift;
	my $hostname = shift;
	my $port = shift;
	my $dispatcher = shift;

	tsrpc::printv( "mainloop : host: $hostname port: $port \n");

# TCPソケット作る
	my( $tcp_mainsock) = new IO::Socket::INET(
#							 LocalAddr => $hostname , 
							 LocalPort => $port ,
							 Proto => 'tcp' , 
							 Listen => 5,
							 Reuse => 1, );

	die "failed to create TCP main socket" unless $tcp_mainsock;

# UDPソケットを作る
	my( $udp_mainsock ) = new IO::Socket::INET(
							LocalAddr => $hostname , 
							LocalPort => $port ,
							Proto => 'udp' );
	die "failed to create UDP main socket" unless $udp_mainsock;

	tsrpc::printv( " TCP created : $tcp_mainsock \n" );
	tsrpc::printv( " UDP created : $udp_mainsock \n" );

# fdset作る TCP
	my $tcp_rfdset = new IO::Select();
	$tcp_rfdset->add($tcp_mainsock );
	my $tcp_efdset = new IO::Select();
	$tcp_efdset->add($tcp_mainsock );

# fdsetを作るUDP
	my $udp_rfdset = new IO::Select();
	$udp_rfdset->add( $udp_mainsock );
	my $udp_efdset = new IO::Select();
	$udp_efdset->add( $udp_mainsock );

# ノンブロッキングSTDIO
	$| = 1;

# acceptループ
	while(1){

# TCP select
		
		my( $new_tcp_readable, $new_tcp_error) = IO::Select->select( $tcp_rfdset,undef,$tcp_efdset,0);

		my $sock;
		foreach $sock ( @$new_tcp_readable ){
			if( $sock == $tcp_mainsock ){		# mainsockと同じだったらacceptする
				my $newsock = $sock->accept();
				$tcp_rfdset->add( $newsock );
			} else {
				$buf = <$sock>;
				tsrpc::printv( "$buf" );
				chomp( $buf );
				if( $buf ){
# ここがエンジンである。ディスパッチルーチン。
					my $output = &$dispatcher( $buf );
					print $sock "$output\n";  
					$sock->flush();
				} 
			}
		}
		foreach $sock ( @$new_tcp_error ){
			$tcp_rfdset->remove( $sock );
			$tcp_efdset->remove( $sock );
			close( $sock );
			print "CLOSED $sock \n";
		}

# UDP select
		my ( $new_udp_readable , $new_udp_error )=IO::Select->select( $udp_rfdset, undef,$udp_efdset,0);
		foreach $sock ( @$new_udp_readable ){
			my $buf ="";
			my $adr = $udp_mainsock->recv( $buf , $max_udp_size );
			if( $verbose ){
				my ($port , $a ) = sockaddr_in( $adr );
				$host = inet_ntoa( $a );
				print  "adr: <" , $host, " "  , $port , ">\n";
			}
			tsrpc::printv( "$buf" );
			chomp( $buf );
			my $output = &$dispatcher( $buf );
			$sock->send( "$output\n" );
		}
		foreach $sock ( @$new_udp_error ){
			if( $sock == $udp_mainsock ){ close( $sock ); }

		}
	}
}



1;


