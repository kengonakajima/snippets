package main

import (
	"fmt";
	)



func main(){
	var b []byte;
	b = make( []byte, 10 )
	b[0] = 1
	b[1] = 10
	fmt.Printf( "%x\n", b )
}
	