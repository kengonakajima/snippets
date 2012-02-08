package main

import (
	"fmt";
	"os";
	"time";
	"strings";

)

type B struct {
	iddddddd int
	idddddd int
}

type A struct {
	id int
	id2 int
	id3 int
	bp *B
}

func main() {
	n := 100
	
	for j :=0; j< n; j++ {

		{
			st := time.Nanoseconds()
			s := "asho"
			for i:= 0; i < 500; i++ {
				s += "asdfasdf"
			}
			et := time.Nanoseconds()
			fmt.Fprintf(os.Stderr, "XXXX %d %d ", (et-st)/1000, len(s) )
		}
		{
			st := time.Nanoseconds()
			var ary []string
			ary = make([]string,500)
			for i:= 0; i < 500; i++ {
				ary[i] = "asdfasdf"
			}
			s := strings.Join(ary,"")
			et := time.Nanoseconds()
			fmt.Fprintf(os.Stderr, "YYYY %d %d ", (et-st)/1000, len(s) )
		}
	}
}

	