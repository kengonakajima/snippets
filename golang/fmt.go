package main

import (
	"fmt"
)

func main() {
	o := ""
	for i:=0;i<1000000;i++ {
		s := fmt.Sprintf( "a%db%d\n", i,i )
		if ( ( i % 100000 ) == 0 ){
			o += s
		}
	}
	fmt.Printf("%s\n", o )
}
