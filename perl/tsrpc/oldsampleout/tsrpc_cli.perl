#
# rpcgen output sample
#
#
#


use strict subs;

require "tsrpc.perl";

use Exporter();
@EXPORT = qw( remotesum2 );



#
# $B%W%m%H%3%k%U%!%$%k$KDj5A$5$l$F$$$k4X?t$NDj5A(B
#
sub remotesum2(@)
{
	return demkstr_int32( 
		call_tsrpctcp( "remotesum_TSRPC", mkstr_string( splice(@_,0,1)) , mkstr_int32_array( splice(@_,1,10) ),"\n" ));
}












