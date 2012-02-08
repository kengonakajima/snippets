use IO::Socket;
use IO::Select;


use strict subs;



#############################################################
#
# クライアント用コード
#
# call_tsrpctcp @
#############################################################
sub call_tsrpctcp(@)
{
	my $str = join( ' ' , @_ );
	print $cmd ;
	return call_through_tcp( $cmd );
}

#
# TCPのサブシステムを使って通信する
# ( $str , $hostname, $port)
sub call_through_tcp($$$)
{
	my $str = chomp(shift);
	my $hostname = shift;
	my $port = shift;

# socket part
	$tsrpc_sock= new IO::Socket::INET( 
							PeerHost => $hostname ,
							PeerPort => $port,
							Proto => 'tcp',
							);
	print $tsrpc_sock , $port , "\n";
	
# main part
	print $tsrpc_sock "$line\n";
	my $ret = <$tsrpc_sock>;

# close part
	close($tsrpc_sock );
	print "socket closed. return message is $ret\n";

# return part
	return $ret;
}


#############################################################
#
# 以下はサーバーのルーチンである。dispatcherはrpcgenが吐く
#
#  TCP select serve
#  ( $hostname, $port )
#############################################################
sub mainloop($$)
{
	my $hostname = shift;
	my $port = shift;

# 普通にソケット作る
	my $mainsock = new IO::Socket::INET(
							 LocalHost => $hostname , 
							 LocalPort => $port ,
							 Proto => 'tcp' , 
							 Listen => 5,
							 Reuse => 1, );

	die "FUCK!" unless $mainsock;

	print $mainsock , " created \n" ;
	

# fdset作る

	my $rfdset = new IO::Select();
	my $rfdset->add($mainsock );
#	my $wfdset = new IO::Select();
#	my $wfdset->add($mainsock );


# acceptループ
	while(1){

# selectは、新しく読めるようになったソケットのハンドルを返してくる
		my $new_readable = IO::Select->select( $rfdset,undef,undef,0);

#から、それをforeachする
		my $sock;
		foreach $sock ( @$new_readable ){

# mainsockと同じだったらacceptする
		if( $sock == $mainsock ){
			my $newsock = $sock->accept();
			$rfdset->add( $newsock );
		} else {
# ちがったらあたらしくつくったソケットへのアクセスだ。
# ソケットが腐ったかどうかは以下で調べることができる。
			$buf = <$sock>;
			chomp( $buf );
			if( $buf ){
# ここがエンジンである。ディスパッチルーチン。
				dispatcher( $buf );
				print $newsock "$buf\n";
			} else {
				$rfdset->remove( $sock );
				close($sock );
			}
		}
	}
}





#############################################################
# 以下は固定のユーティリ。クライアントでもサーバでも使う
#
# 
#
sub escape($)
{
	$_ = shift;

	s/ /\\S/g;
	s/\n/\\n/g;

	return $_;
}

sub descape($)
{
	$_ = shift;

	s/\\n/\n/g;
	s/\\S/ /g;

	return $_;
}



# 入力はperlの生なスカラで、出力はエスケープされたネットワーク文字列表現。
sub mkstr_long32($){ return unpack( "l" , pack("l",shift()));  }
sub mkstr_u_long32($){	return unpack( "L" , pack("L",shift()));  }
sub mkstr_int32($){	return unpack( "i" , pack("i" , shift()));  }
sub mkstr_u_int32($){	return unpack( "I" , pack("I" , shift()));  }
sub mkstr_short16($){	return unpack( "s" , pack("s" , shift()));  }
sub mkstr_u_short16($){	return unpack( "S" , pack("S" , shift()));  }
sub mkstr_char8($){	return unpack( "c" , pack("c" , shift()));  }
sub mkstr_u_char8($){	return unpack( "C" , pack("C" , shift()));  }
sub mkstr_float32($){	return unpack( "f" , pack("f" , shift()));  }
sub mkstr_double64($){	return unpack( "d" , pack("d" , shift()));  }
sub mkstr_string($){ return escape( shift()); }
sub mkstr_long32_array(@){ return mkstr_array_base(  \&mkstr_long32 , 0 , @_ ); }
sub mkstr_u_long32_array(@){ return mkstr_array_base( \&mkstr_u_long32 , 0 ,@_ ); }
sub mkstr_int32_array(@){ return mkstr_array_base( \&mkstr_int32 , 0 , @_ ); }
sub mkstr_u_int32_array(@){ return mkstr_array_base( \&mkstr_u_int32 , 0 ,@_ ); }
sub mkstr_short16_array(@){ return mkstr_array_base( \&mkstr_short16 , 0 , @_ ); }
sub mkstr_u_short16_array(@){ return mkstr_array_base( \&mkstr_u_short16 , 0 ,@_ ); }
sub mkstr_char8_array(@){ return mkstr_array_base( \&mkstr_char8 , 0 , @_ ); }
sub mkstr_u_char8_array(@){ return mkstr_array_base( \&mkstr_u_char8 , 0 ,@_ ); }
sub mkstr_float32_array(@){ return mkstr_array_base( \&mkstr_float32 , 0 , @_ ); }
sub mkstr_double64_array(@){ return mkstr_array_base( \&mkstr_double64 , 0  , @_ ); }
sub mkstr_string_array(@){ return mkstr_array_base( \&mkstr_string , 0 , @_ ); }
# 配列を作る場合の基本関数
# ( \&func , $defaultvalue , @_ )
sub mkstr_array_base($$@)
{
		my $func = shift;
		my $defaultvalue = shift;
		my $size = @_;	

		my @out;
		for( $i=0;$i<$size;$i++){
			$_[$i] = $defaultvalue if( undef( $_[$i] ) || $_[$i] eq "" );
			push( @out,  \&func( $_[$i] ) );
		}
		return join( ' ', @out );
}


# 入力は、ネットワーク表現で、出力はperlの生なデータ。
sub demkstr_long32($){ return unpack( "l" , pack( "l" , shift())); }
sub demkstr_u_long32($){ return unpack( "L" , pack("L",shift()));  }
sub demkstr_int32($){ return unpack( "i" , pack("i" , shift()));  }
sub demkstr_u_int32($){	return unpack( "I" , pack("I" , shift()));  }
sub demkstr_short16($){	return unpack( "s" , pack("s" , shift()));  }
sub demkstr_u_short16($){ return unpack( "S" , pack("S" , shift()));  }
sub demkstr_char8($){	return unpack( "c" , pack("c" , shift()));  }
sub demkstr_u_char8($){	return unpack( "C" , pack("C" , shift()));  }
sub demkstr_float32($){	return unpack( "f" , pack("f" , shift()));  }
sub demkstr_double64($){ return unpack( "d" , pack("d" , shift()));  }
sub demkstr_string($){ return descape( shift()); }
sub demkstr_long32_array(@){ return demkstr_array_base( \&demkstr_long32 ,@_ ); }
sub demkstr_u_long32_array(@){ return demkstr_array_base( \&demkstr_u_long32 ,@_ ); }
sub demkstr_int32_array(@){ return demkstr_array_base( \&demkstr_int32 ,@_ ); }
sub demkstr_u_int32_array(@){ return demkstr_array_base( \&demkstr_u_int32 ,@_ ); }
sub demkstr_short16_array(@){ return demkstr_array_base( \&demkstr_short16 ,@_ ); }
sub demkstr_u_short16_array(@){ return demkstr_array_base( \&demkstr_u_short16 ,@_ ); }
sub demkstr_char8_array(@){ return demkstr_array_base( \&demkstr_char8 ,@_ ); }
sub demkstr_u_char8_array(@){ return demkstr_array_base( \&demkstr_u_char8 ,@_ ); }
sub demkstr_float32_array(@){ return demkstr_array_base( \&demkstr_float32 ,@_ ); }
sub demkstr_double64_array(@){ return demkstr_array_base( \&demkstr_double64 ,@_ ); 
sub demkstr_string_array(@){ return demkstr_array_base( \&demkstr_string ,@_ ); }

# 配列のデコードは、リストの項目を処理するだけ
# 引数の配列のサイズは上のルーチンによって正しくなっているとする
sub demkstr_array_base($@){
	my $func = shift;
	my @out ,$a;
	for $a ( @_ ){
		push (@out , \&func($a) );
	}
	return @out;
}

