

$|=1;


$a = shift( @ARGV );


if( $a eq 'lock' ){
	# do flock
	open( L , "hoge" ) || die "cannot open\n";
	flock( L , 2 ) || die "cannot lock :$!\n";
	print "done\n";
	while(1){
	}
	close( L);
} else {
	# do unlock
	open( L , "hoge" ) || die "cannot open\n";
	flock( L , LOCK_UN ) || die "cannot unlock\n";
	print "done\n";
	close(L);
}
