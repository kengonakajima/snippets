
print join(' ',@INC);

use IO::Socket;
use Net::FTP;



# chdir "/tmp";


#
#  $B0J2<$NNc$G%U%!%$%k$r<h$C$F$3$l$k$N$@(B
#
#

#$ftp = Net::FTP->new( "210.140.231.61" );
$ftp = Net::FTP->new( "jap" );

#$ftp->login( "jss_ls" , "W1naMp!" );
$ftp->login( "ringo" , "hoge111" );

print STDERR "logged in \n";

$ftp->cwd( "/opt/ringo/" );

print STDERR "cwd.\n";


#
#  $B0J2<$O(Bget$B$G!"@.8y$7$?$i(B$result $B$K$O%U%!%$%kL>$,F~$k(B
#  $B<:GT$7$?$i6u(B
#

$fuck = $ftp->type( "binary" );

print STDERR "set type\n";

$result = $ftp->get( "aho" );

print STDERR "got file?\n";


print "result is: $result \n";


$ftp->quit;
