
print join(' ',@INC);

use IO::Socket;
use Net::FTP;



# chdir "/tmp";


#
#  以下の例でファイルを取ってこれるのだ
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
#  以下はgetで、成功したら$result にはファイル名が入る
#  失敗したら空
#

$fuck = $ftp->type( "binary" );

print STDERR "set type\n";

$result = $ftp->get( "aho" );

print STDERR "got file?\n";


print "result is: $result \n";


$ftp->quit;
