use strict subs;



use IO::Socket;

$|=1;

$sock= new IO::Socket::INET( PeerAddr => 'localhost' ,
							PeerPort => 23,
							Proto => 'tcp',
							);
print "SOCK:" , $sock , " $@\n";

while($lin = <>)
{
	print $sock "Fucking message is : $lin ";

}

close($sock );
print "closed.\n";

								
							
