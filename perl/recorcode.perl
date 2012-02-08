#!/usr/local/bin/perl



@sufs= ( "c" , "h" , "cpp" , "perl" , "java" , "el" , "pl" , "sh",
		"txt" , "html" , "tex" , "htm" , "doc" );

$dir = ".";
$nkfopt = "-s";

if( @ARGV == 0){
	die ("Usage : $0 [-s/-e/-j] directory\n" );
}

foreach $arg ( @ARGV )
{
	if ($arg =~ /^-(.+)/ ){
		$nkfopt = $arg;
	} else {
		$dir = $arg;
	}
}



$ct = time( );

$tmpfile = "/tmp/fucktmp$ct$PID" ;
$tmpfile2 = "/tmp/fucktmp_$ct$PID";

system( "find $dir -print > $tmpfile" );

if( !open( IN , "$tmpfile" ) ){die("cannot open $tmpfile\n" );}


while( <IN>)
{
	chomp;

	@sp = split( '\.' , $_ );
	$fsuf = $sp[@sp-1];

	
	print $_ , " : ";
	print $fsuf;

	foreach $suf ( @sufs ){
		if( $suf eq $fsuf ){
			print  " : nkf $nkfopt ";
			system( "nkf $nkfopt $_ > $tmpfile2" );
			system( "mv $tmpfile2 $_" );
			last;
		}
	} 



	print "\n";

}

system( "rm $tmpfile" );

