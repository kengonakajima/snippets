#
# $B$G!<$?$r$b$8$K$9$k$N(B
# perl$BMQ(B
#

use strict subs;


# print remotestrcapcount( "This is a SAMPLE string.\n" , 10 ) ,"\n";
print remotesum2( "answer",( 10 , 20 , 30 , 44 , 5 , 60 ,77,88,90,100) );



#
# $B0J2<$N=hM}$O%f!<%6Dj5A(B
#
# ( str , int32[10] )
sub remotesum2_TSRPC(@)
{								
	$fuck= shift;
	print "FUCK is: $fuck\n";
	my $a=0 , $out=0;
	for $a ( @_ ){
		print "A is : $a\n";
		$out += $a;
	}
	return $out;
}


#
# $B0J2<$N4X?t$O(Brpcgen$B$,=P$9%3!<%I(B
#
# $ is from net
sub dispatcher($)
{
	$_ = shift;
	my @args = split( ' ' , $_ );
	my $funcname = shift( @args );
	my $out;

	if( $funcname eq "remotesum_TSRPC" ){
		$out = mkstr_int32( remotesum2_TSRPC( demkstr_string( splice(@args,0,1)) ,demkstr_int32_array( splice( @args,1,10))));
	} else {
		$out = "no_such_service\n";
	}
	return $out;
}
# $B0J2<$O(Brpcgen$B$,=P$9%3!<%I(B.splice$B$,%]%$%s%H(B
# $B$+$J$i$:(Bsplice$B$,$O$$$k(B
sub remotesum2(@)
{
	return demkstr_int32( 
		call_tsrpctcp( "remotesum_TSRPC", mkstr_string( splice(@_,0,1)) , mkstr_int32_array( splice(@_,1,10) ),"\n" ));
}


#
# $B0J2<$O8GDj$N%f!<%F%#%j(B
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
sub mkstr_int32_array(@)		# $$B$O%5%$%:$G!"(B @$B$OG[Ns!#(B
{
	my $size = @_;	
	print "SIZ:$size\n";

	my($out);

	for( $i=0;$i<$size;$i++){
		if( undef( $_[$i] ) || $_[$i] eq "" ){
			push( @out ,  "0" );
		} else {
			push( @out,  mkstr_int32( $_[$i] ) );
		}
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

# $BG[Ns$N%G%3!<%I$O!"%j%9%H$N9`L\$r=hM}$9$k$@$1(B
# $B0z?t$NG[Ns$N%5%$%:$O>e$N%k!<%A%s$K$h$C$F@5$7$/$J$C$F$$$k$H$9$k(B
sub demkstr_int32_array(@)
{
	my @out, $a;
	for $a ( @_ ){	
		push( @out , demkstr_int32( $a ) );
	}
	return @out;
}


#
# $B%/%i%$%"%s%HMQ%3!<%I(B
#
# call_tsrpctcp 
sub call_tsrpctcp(@)
{
	my($cmd) = join( ' ' , @_ );
	print $cmd ;
	return dummy_tcp_subsystem( $cmd );
}
#
# $B%@%_!<$J%H%i%s%9%]!<%H(B
#
sub dummy_tcp_subsystem($)
{
	return dispatcher( shift );
}
