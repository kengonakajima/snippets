#
#
# output of rpcgen.perl server side
#
#
#




require tsrpc.perl



mainloop("localhost" , 10001 );





#
# $B0J2<$N4X?t$O(Brpcgen$B$,=P$9%3!<%I(B
#
# $ is from net
sub dispatcher($)
{
	$_ = shift;
	my @args = split( ' ' , $_ );
	my $funcname = shift( @args );
	my $out;

	if( $funcname eq "remotesum_TSRPC" ){
		$out = mkstr_int32( remotesum2_TSRPC( demkstr_string( splice(@args,0,1)) ,demkstr_int32_array( splice( @args,1,10))));
	} else {
		$out = "no_such_service\n";
	}
	return $out;
}







