package main
import (
	"fmt"
	"time"
	
)

type Entry struct {
	val int
	next *Entry
}

func main() {
	var ary = make( []*Entry, 1024 * 1024 )
	
	total :=0
	t := time.Nanoseconds()
	for i:=0;i<10000000;i++ {
		ind := i % (1024*1024) 
		if( ary[ ind ] == nil  ){
			ary[ ind ] = new(Entry)
			ary[ ind ].val = i
		} else {
			var e *Entry
			e = new(Entry)
			e.val = i
			ary[ind].next = e
			e.next = nil
		}
	}
	e := time.Nanoseconds()
	fmt.Printf("%d %d\n",total, ( e - t ) / 1000 )
	t = time.Nanoseconds()
	total=0
	for i:=0;i<100000000;i++ {
		ind := i % (1024*1024) 		
		var cur *Entry
		cur = ary[ind]
		for {
			if cur == nil { break }

			total += cur.val
			cur = cur.next
		}
	}
	e = time.Nanoseconds()
	fmt.Printf("%d %d\n",total, ( e - t ) / 1000 )	
}

