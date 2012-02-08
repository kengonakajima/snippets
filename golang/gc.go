package main

import (
	"fmt";
	"time";
	"os";
)


type Hoge struct {
	bbb string
	i0 int
	i1 int
	i2 int
}

func main() {
	var m map[int]*Hoge

	m = make( map[int]*Hoge )

	for j:=0; j<1000;j++ {
		for i:=0; i< 100000; i++ {
			var h *Hoge
			h = new(Hoge)
			m[i] = h
		}
		time.Sleep(100*1000)
		fmt.Fprintf( os.Stderr, ".")
		for i:=0; i< 100000; i++ {
			m[i]=nil
		}
	}	
}
