use IO::Socket;
use IO::Select;


use strict subs;



#############################################################
#
# $B%/%i%$%"%s%HMQ%3!<%I(B
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
# TCP$B$N%5%V%7%9%F%`$r;H$C$FDL?.$9$k(B
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
# $B0J2<$O%5!<%P!<$N%k!<%A%s$G$"$k!#(Bdispatcher$B$O(Brpcgen$B$,EG$/(B
#
#  TCP select serve
#  ( $hostname, $port )
#############################################################
sub mainloop($$)
{
	my $hostname = shift;
	my $port = shift;

# $BIaDL$K%=%1%C%H:n$k(B
	my $mainsock = new IO::Socket::INET(
							 LocalHost => $hostname , 
							 LocalPort => $port ,
							 Proto => 'tcp' , 
							 Listen => 5,
							 Reuse => 1, );

	die "FUCK!" unless $mainsock;

	print $mainsock , " created \n" ;
	

# fdset$B:n$k(B

	my $rfdset = new IO::Select();
	my $rfdset->add($mainsock );
#	my $wfdset = new IO::Select();
#	my $wfdset->add($mainsock );


# accept$B%k!<%W(B
	while(1){

# select$B$O!"?7$7$/FI$a$k$h$&$K$J$C$?%=%1%C%H$N%O%s%I%k$rJV$7$F$/$k(B
		my $new_readable = IO::Select->select( $rfdset,undef,undef,0);

#$B$+$i!"$=$l$r(Bforeach$B$9$k(B
		my $sock;
		foreach $sock ( @$new_readable ){

# mainsock$B$HF1$8$@$C$?$i(Baccept$B$9$k(B
		if( $sock == $mainsock ){
			my $newsock = $sock->accept();
			$rfdset->add( $newsock );
		} else {
# $B$A$,$C$?$i$"$?$i$7$/$D$/$C$?%=%1%C%H$X$N%"%/%;%9$@!#(B
# $B%=%1%C%H$,Ie$C$?$+$I$&$+$O0J2<$GD4$Y$k$3$H$,$G$-$k!#(B
			$buf = <$sock>;
			chomp( $buf );
			if( $buf ){
# $B$3$3$,%(%s%8%s$G$"$k!#%G%#%9%Q%C%A%k!<%A%s!#(B
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
# $B0J2<$O8GDj$N%f!<%F%#%j!#%/%i%$%"%s%H$G$b%5!<%P$G$b;H$&(B
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



# $BF~NO$O(Bperl$B$N@8$J%9%+%i$G!"=PNO$O%(%9%1!<%W$5$l$?%M%C%H%o!<%/J8;zNsI=8=!#(B
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
# $BG[Ns$r:n$k>l9g$N4pK\4X?t(B
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


# $BF~NO$O!"%M%C%H%o!<%/I=8=$G!"=PNO$O(Bperl$B$N@8$J%G!<%?!#(B
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

# $BG[Ns$N%G%3!<%I$O!"%j%9%H$N9`L\$r=hM}$9$k$@$1(B
# $B0z?t$NG[Ns$N%5%$%:$O>e$N%k!<%A%s$K$h$C$F@5$7$/$J$C$F$$$k$H$9$k(B
sub demkstr_array_base($@){
	my $func = shift;
	my @out ,$a;
	for $a ( @_ ){
		push (@out , \&func($a) );
	}
	return @out;
}

