#
#
# perl simple  RPC package
#
#  $B%3!<%I$NCGJR$r$d$j$H$j$9$kC1=c$G6/NO$J(BRPC$B$G$"$k!#(B
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#

package PR;

use Exporter;

@ISA = ( 'Exporter');		
@EXPORT_OK = ( 'perlRPCTest' , 'ahofunc' );



sub perlRPCTest
{

	print shift() , "\n";


}