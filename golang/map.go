package main

import (
	"fmt";
	"time";
)

func main(){
	m := make( map[int]int, 1000000)
	n := 1000000

	s := time.Nanoseconds()
	for i := 0; i < n; i++ {
		m[i]=i*10
	}
	e := time.Nanoseconds()
	println( "dt:" , (e-s)/1000/1000)
	
	s = time.Nanoseconds()
	tot := 0
	for i := 0; i < n; i++ {
		tot += m[i]
	}
	e = time.Nanoseconds()
	println( "dt:" , (e-s)/1000/1000)	

	///////////
	a := make( []int,1000000)

	s = time.Nanoseconds()
	for i := 0; i < n; i++ {
		a[i]=i*10
	}
	e = time.Nanoseconds()
	println( "dt:" , (e-s)/1000/1000)

	s = time.Nanoseconds()
	tot = 0
	for i := 0; i < n; i++ {
		tot += a[i]
	}
	e = time.Nanoseconds()
	println( "dt:" , (e-s)/1000/1000)
	
}

func q(){
	var h map[int]string
	h = make( map[int]string )

	fmt.Printf("%d\n", len(h))

	h[0] = "aho"
	fmt.Printf("%d\n", len(h))	
}
