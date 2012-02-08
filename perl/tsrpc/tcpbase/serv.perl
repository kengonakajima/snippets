#
#
#  TCP select server
#

use IO::Socket;
use IO::Select;

# 普通にソケット作る
$mainsock = new IO::Socket::INET(
							 LocalHost => "localhost" , 
							 LocalPort => 40010,
							 Proto => 'tcp' , 
							 Listen => 5,
							 Reuse => 1, );

die "FUCK!" unless $mainsock;

print $mainsock , "\n" ;


# fdset作る

$rfdset = new IO::Select();
$rfdset->add($mainsock );



# acceptループ
while(1){

# selectは、新しく読めるようになったソケットのハンドルを返してくる
	($new_readable)=IO::Select->select( $rfdset,undef,undef,0);

#から、それをforeachする
	foreach $sock ( @$new_readable ){

# mainsockと同じだったらacceptする
		if( $sock == $mainsock ){
			$newsock = $sock->accept();
			$rfdset->add( $newsock );
		} else {
# ちがったらあたらしくつくったソケットへのアクセスだ。
# ソケットが腐ったかどうかは以下で調べることができる。
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

