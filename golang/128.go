package main

import (
	"fmt";
	"os";
	"time";
)

// OIDはbyte[]だった
type IdStruct struct {
	
	lower int64
}



func main() {
	var m64 = make(map[int64]int,10000)
	var m32 = make(map[int32]int,10000)
	var ms = make(map[string]int,10000)
	t := time.Nanoseconds()

	for i  :=0; i < 1000000; i++ {
		m64[ int64(i)]=i
	}
	e := time.Nanoseconds()
	fmt.Fprintf( os.Stderr, "64: %d dt:%d\n", m64[1],  (e - t)/1000)
	t = e
	total := 0
	for i :=0; i < 1000000; i++ {
		total += m64[ int64(i)]
	}
	e = time.Nanoseconds()
	fmt.Fprintf( os.Stderr, "64lookup: %d dt:%d\n", total,  (e - t)/1000)
	
	t = e
	for i  :=0; i < 1000000; i++ {
		m32[ int32(i)]=i
	}
	e = time.Nanoseconds()
	fmt.Fprintf( os.Stderr, "32: %d dt:%d\n", m32[1], (e - t )/1000)

	t = e
	total=0
	for i  :=0; i < 1000000; i++ {
		total+=m32[ int32(i)]
	}
	e = time.Nanoseconds()
	fmt.Fprintf( os.Stderr, "32lookup: %d dt:%d\n", total, (e - t )/1000)
	
	t = e
	var msi = make(map[int]string)
	for i  :=0; i < 1000000; i++ {
		s := fmt.Sprintf( "%d%d", i,i )
		msi[i]=s
	}
	e = time.Nanoseconds()
	fmt.Fprintf( os.Stderr, "msi:  dt:%d\n",  (e - t )/1000)
	t = e
	for i  :=0; i < 1000000; i++ {
		ms[ msi[i] ] = i
	}
	e = time.Nanoseconds()
	fmt.Fprintf( os.Stderr, "s: %d dt:%d\n", ms[ "11" ], (e - t )/1000)

	t = e
	total=0
	for i  :=0; i < 1000000; i++ {
		total+=ms[ msi[i] ] 
	}
	e = time.Nanoseconds()
	fmt.Fprintf( os.Stderr, "slookup: %d dt:%d\n", total, (e - t )/1000)
	
}

	
