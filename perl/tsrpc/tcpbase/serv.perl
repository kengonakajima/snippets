#
#
#  TCP select server
#

use IO::Socket;
use IO::Select;

# $BIaDL$K%=%1%C%H:n$k(B
$mainsock = new IO::Socket::INET(
							 LocalHost => "localhost" , 
							 LocalPort => 40010,
							 Proto => 'tcp' , 
							 Listen => 5,
							 Reuse => 1, );

die "FUCK!" unless $mainsock;

print $mainsock , "\n" ;


# fdset$B:n$k(B

$rfdset = new IO::Select();
$rfdset->add($mainsock );



# accept$B%k!<%W(B
while(1){

# select$B$O!"?7$7$/FI$a$k$h$&$K$J$C$?%=%1%C%H$N%O%s%I%k$rJV$7$F$/$k(B
	($new_readable)=IO::Select->select( $rfdset,undef,undef,0);

#$B$+$i!"$=$l$r(Bforeach$B$9$k(B
	foreach $sock ( @$new_readable ){

# mainsock$B$HF1$8$@$C$?$i(Baccept$B$9$k(B
		if( $sock == $mainsock ){
			$newsock = $sock->accept();
			$rfdset->add( $newsock );
		} else {
# $B$A$,$C$?$i$"$?$i$7$/$D$/$C$?%=%1%C%H$X$N%"%/%;%9$@!#(B
# $B%=%1%C%H$,Ie$C$?$+$I$&$+$O0J2<$GD4$Y$k$3$H$,$G$-$k!#(B
			$buf = <$sock>;
			if( $buf ){
				print "[$buf]\n";
				print $sock "$buf\n";
				$sock->flush();

			} else {
				$rfdset->remove( $sock );
				close($sock );
			}
		}
	}
}

