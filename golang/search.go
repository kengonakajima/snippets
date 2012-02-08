// search api using interface

package main
import (
	"fmt";
//	"time";
)


var ch chan []int

func searcher_proc() {

	var db map[int]int

	db = make( map[int]int, 10000 )
	
	// init
	for i := 0; i < 10000; i++ {
		db[i] = i * 2
	}
	
	// run
	for {
		req := <- ch

		var out []int
		out = make( []int, len(req) )
		for i, x := range req {
			out[i] = db[x]
		}
		ch <- out
	}
	
}

var outch chan int

func main() {

	ch = make(chan []int)

	go searcher_proc()

	outch = make(chan int)

	n := 100
	m := 10000
	for i := 0; i < n; i++ {
		go service_proc(i,m)
	}


	for i := 0; i < n*m; i++ {
		res := <- outch
		if ( i % ((n*m)/10) ) == 0 {
			fmt.Printf("i:%d %d\n", i, res )
		}
	}

	

}

func service_proc(i int,n int) {
	for l := 0; l < n; l++ {
    	ids := []int{ 10,20,30 }	
		ch <- ids
		result := <- ch

		var total int
		
		for _,r := range result {
			total += r
		}
		outch <- total
	}

}

	
