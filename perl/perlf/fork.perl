
print "starttest \n";

$SIG{CHLD} = waiter;

$p =  fork();


if( $p == 0 )
{
	print "I am child....\n";
	sleep(5);
	print ".... woke up and finish.\n";
	exit(0);
	
} else {
	print " I am parent.wait for my child wakes up\n";
	for(;;){
		#	waitpid( $p ,0 );
		print "$aaa $i\n";
		$i++;
		sleep(1);
		if( $aaa ne '' ){last;}
		
	}	
}

print "program end\n";

#
sub waiter()
{
	print "start wait\n";
	wait();
	print "wait end\n";
	$aaa = 'asdfasdf';
}
