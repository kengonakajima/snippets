#
#
# tsrpc client module
#
#


package tsrpc_client;



use Symbol;








sub new 
{
		print join( '/' , @_ ) , "\n";

		my $type = shift;
		my $host = shift;
		my $port = shift;


		my $class = ref( $type ) || $type || "tsrpc_client";

		my $fh = gensym();
		my $me = bless $fh , $class;


		${*$fh}{"host"} = $host;
        ${*$fh}{"port"} = $port;

		print "CLASS: $class $host $port \n";
		return $me;

}



sub out
{
	my $fh = shift;





	print "HOST: ${*$fh}{host}  PORT: ${*$fh}{port}\n";
}



1;
