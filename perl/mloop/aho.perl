system("date");

for($i=0;$i<(65536*65535);$i++){
	if(($i%1000000)==0){print STDERR "$i\n";}
}

system("date");
