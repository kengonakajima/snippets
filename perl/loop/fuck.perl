
open( O , ">aaaa");

for($i=0;$i<100000;$i++){
	print O  "to be written in\n";
}

close(O);

