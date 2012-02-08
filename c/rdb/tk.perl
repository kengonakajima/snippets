
while($fn=<@ARGV>){

	open( IN , "<$fn");

	@a =<IN>;
	close(IN);
	foreach $i ( @a ){
		chomp( $i );
		@b = split( /[\b\s,:;\/*()\\.\[\]<>_#$\%&'~=\$\"-]+/ , $i );
		foreach $j ( @b ){
			next if($j eq'');
			print $j,"\n";
		}
	}

}
