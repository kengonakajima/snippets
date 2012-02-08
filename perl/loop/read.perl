open( IN , "aaaa" );

open( OUT , ">readwri" );

#while(<IN>)
#{
	print OUT <IN>;

#}
close(OUT);
