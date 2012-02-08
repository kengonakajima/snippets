
use strict subs;



use IO::Socket;

$|=1;

$sock= new IO::Socket::INET( PeerAddr => 'localhost' ,
							PeerPort => 10100,
							Proto => 'udp',
							);
print "SOCK:" , $sock , " $@\n";



$sock->send( "a" );
$sock->send( "This is a udp message!\n" );
$sock->send( "This is a udp message!\n" );






close($sock );
print "closed.\n";

								
							
