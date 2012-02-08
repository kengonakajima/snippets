// ちゃネルの順番が保証されるかのテスト, gomaxproc にかんけいないく、動きとしては保証されてるように見える。


package main

import (
	"fmt";
	"runtime";
	"time";
	"os";
	"log";
	)

var outch chan int
var ch chan int64

func sharedgoro () {  // runs only one, ( uses DB connection pool )
	for {
		i := <- ch
		ch <- i
	}
}

func workgoro ( id int ) {

	n := 100
	for i := 0; i < n; i++ {
		t := time.Nanoseconds()
		ch <- t
		s := <- ch
		if s != t {
			log.Exit( "fatal" )
		}
	}
	fmt.Fprintf( os.Stderr, "%d ", id )
	outch <- id
}

func main() {
	runtime.GOMAXPROCS(2) 
	n := 10000
	
	outch = make(chan int,n) // to confirm result
	ch = make(chan int64)

	go sharedgoro()

	for i := 0; i< n; i++ {
		workgoro(i)
	}

	for i := 0; i< n; i++ {
		ooo := <- outch
		if i != ooo {
			log.Exit("fatal2")
		}
	}
}
