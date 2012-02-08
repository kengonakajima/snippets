


$a = $ARGV[0];

print $a , "\n";

open( IN , "|ping $a -c 10 -s 10" );

while(1){
	my $line = <IN>;
	if( $line =~ /from/ ){
		print "alive $a\n";
		exit(0);
	}
}

close(IN);

print "not alive $a\n";

