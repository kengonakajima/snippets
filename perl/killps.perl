#!/usr/local/bin/perl 



    
if( @ARGV != 1 ){
	die("usage: killps psname\n" );
}


# print "[$ARGV[0]] arg\n";

$arg = $ARGV[0];

open( IN , "ps ax|" );      # linux ps gives PID in first token

while( $line = <IN> ){

	if( $line =~ /\s$arg\s/ ){
		print $line;	
		@l = split( /\s+/ , $line );

		$pid = $l[0];
		if( $pid == $$ ){ next; }
		
		$comline = "kill $pid";
		system( $comline );
		print "tried to kill process $pid\n";

	}

}

