

open( OUT , ">$ARGV[0]");

@b = ( 'a','b','c','d','e','f','g','h',
	  'i','j','k','l','m','n','o','p','q',
	  'r','s','t','u','v','w',
	  'x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M',
	  'N','O','P','Q','R','S',
	  'T','U','V','W','X','Y','Z','0','1','2','3','4','5','6','7','8','9','-');
#	  '+','&','%','#',
#	  '$','(',')',' ','<','>','_','{','}','@' );



for($i=0;$i<8000000;$i++){
	print makew() , "\n",
}

close(OUT);

sub makew()
{
	my $i;
	my $o ="";
	my $e = int( 2 * rand() ) + 2;
	for($i=0;$i<$e;$i++){
		$o .= $b[ int(( @b) * rand() ) ];
	}

	return $o;
}

