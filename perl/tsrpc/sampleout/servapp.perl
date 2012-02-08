




use tsrpc_server;

# $ts = tsrpc_server->new(  10000 );
$ts = tsrpc_server->new(  "lsdev" ,10000 );

$ts->start();

# /* not reached */





#
#  The function to be called remote
#
sub remotesum2
{
	my $aho = shift;
	print $aho ,"\n";
	my $a;
	my $out;
	for $a ( @_ ){
		$out += $a;
	}
	$out;
}


sub udpcat
{
	my $a = shift;
	my $b = shift;
	$a .= $b;

	print "AAAAAAA: $a\n";
	return $a;
}
