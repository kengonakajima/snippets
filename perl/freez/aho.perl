#
#
#
#freezeThaw のサンプ
#
#
#

use FreezeThaw qw( freeze thaw );

$msg = freeze( '<' , "hoge","aho" ,"asdfa" ,"asdf");

print $msg , "\n";

( $fuck ,$shit ,$godemn ,$dead) =thaw( $msg );

print $fuck ,"\n";
print $shit ,"\n";
print $godemn ,"\n";
print $dead ,"\n";


