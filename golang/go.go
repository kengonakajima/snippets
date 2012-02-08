package main

import (
	"fmt";
//	"os";
	"time";
	"runtime";

)

func f() {
	for n :=0; n < 10000000; n ++ {
		if ( n % 1000000 ) == 0 {
			fmt.Printf( "%d\n",n)
		}
	}
}


func main() {
	fmt.Printf( "%d\n", runtime.GOMAXPROCS(1) )
	
	n:=10
	for i:=0; i< n; i ++ {
		go f()
	}

	time.Sleep(1000*1000*1000)
}
