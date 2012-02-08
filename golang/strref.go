package main

import (
	"fmt"
	"time"
)

var total = 0

func f( s *string ) {
	total += int((*s)[0])
}

func main() {
	n := 2000
	var s string 
	for i:= 0; i<n;i++ {
		s += "aksjdlfakjsdhflkajsdhlfkjashdlkf" 
	}
	t := time.Nanoseconds()
	for i := 0; i< 10000000;i++ {
		f(&s)
	}
	e := time.Nanoseconds()
	fmt.Printf( "%d\n", ( e - t ) / 1000 )
	
	
}
