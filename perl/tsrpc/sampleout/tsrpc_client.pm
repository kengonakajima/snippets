package tsrpc_client;

use Symbol;
use IO::Socket;
use IO::Select;

use strict subs;

use tsrpc;

#
# ( $object , @args )
#
sub remotesum2($@)
{
	my $fh = shift;
	@_ = tsrpc::fillargs( 11 , @_ );
	return tsrpc::demkstr_int32( tsrpc::call_tsrpctcp( 
						  ${*$fh}{"tcpsocket"} ,
						  "remotesum2",
						  tsrpc::mkstr_string( splice(@_,0,1)) ,
						  tsrpc::mkstr_int32_array( splice(@_,0,10) ),
						  "\n" 
    ));
}
sub udpcat($@)
{
	my $fh = shift;
#	@_ = tsrpc::fillargs( 2 , @_ );
	return tsrpc::demkstr_string( tsrpc::call_tsrpcudp(
						${*$fh}{"udpsocket"} ,
						"udpcat",
						tsrpc::mkstr_string( splice( @_ , 0, 1 ) ) ,
						tsrpc::mkstr_string( splice( @_ , 0, 1 ) ) ,
						"\n"
	));
}

#   create new client handler
#   new ($hostname , $port )
#
sub new 
{
	my $type = shift;
	my $host = shift;
	my $port = shift;

	my $class = ref( $type ) || $type || "tsrpc_client" ;
	my $fh = gensym();
	my $me = bless $fh, $class;

	${*$fh}{"host"} = $host;
    ${*$fh}{"port"} = $port;
	${*$fh}{"tcpsocket"} = new IO::Socket::INET( 
							PeerAddr => $host ,
							PeerPort => $port,
							Proto => 'tcp' );
	if( ${*$fh}{"tcpsocket"} == undef ){
		die "failed to create TCP socket\n";
	}
 	${*$fh}{"udpsocket"} = new IO::Socket::INET(
							PeerAddr => $host , 
							PeerPort => $port ,
							Proto => 'udp' );
	if( ${*$fh}{"udpsocket"} == undef ){
		die "failed to create UDP socket\n";
	}

	return $me;
}

#
#
#
sub destroy
{
	my $fh = shift;

	${*$fh}{"host"} = "";
	${*$fh}{"port"} = "";
	close(${*$fh}{"tcpsocket"} );
	close(${*$fh}{"udpsocket"} );

	return 1;
}



1;







