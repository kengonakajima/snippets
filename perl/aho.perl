

$code = '$aho=10;$f=20;@shit=($aho,$f); fuck(@shit);';


eval( $code );

sub fuck
{

	foreach $a ( @_ )
	{
		print "A:" , $a , "\n" ;
	}

}





