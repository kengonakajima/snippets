#
#
#
# $B6uGrJ8;z$G$/$.$k$+$i!"6uGr$r%(%9%1!<%W$9$kI,MW$,$"$k!#(B
# $B%(%9%1!<%W$9$kJ8;z(B($B$H$/$Y$D$J$$$_$N$d$D(B)$B$O0J2<(B
# ' '
# '\n'

print escape( $fuck = "aho is fuck.,\nnext line is also fuck.\n" );

print descape( $fuck );



sub escape($)
{
	$_ = shift;

	s/ /\\ /g;
	s/\n/\\n/g;

	return $_;
}

sub descape($)
{
	$_ = shift;

	s/\\n/\n/g;
	s/\\ / /g;

	return $_;
}


