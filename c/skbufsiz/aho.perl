@a = `netstat -t|grep tcp`;


$total=0;

foreach $i ( @a )
{
	
	@b = split( /[ ]+/ , $i );

	$total += $b[2];
}

print $total , "\n";
