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
# プロトコルファイルに定義されている関数の定義
#
sub remotesum2(@)
{
	return demkstr_int32( 
		call_tsrpctcp( "remotesum_TSRPC", mkstr_string( splice(@_,0,1)) , mkstr_int32_array( splice(@_,1,10) ),"\n" ));
}












