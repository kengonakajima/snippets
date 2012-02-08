#
#
# perl simple  RPC package
#
#  コードの断片をやりとりする単純で強力なRPCである。
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