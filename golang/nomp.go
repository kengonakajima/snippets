// mempool

package main

import (
	"log"
	"time"
)

type Player struct {
	id [100]int
}


// new/makeの速度検証
func main() {
	k := 100
	n := 1000000
	p := make( []*Player, n )


	

	for j:=0;j<k;j++ {
		log.Printf( "start\n")
		t := time.Nanoseconds()
		for i:=0;i<n;i++ {
			p[i] = new(Player)
		}
		for i:=0;i<n;i++ {
			p[i].id[99] = i
			p[i].id[0] = i
		}
		e := time.Nanoseconds()
		log.Printf( "end: %d\n", ( e - t ) / 1000 / 1000 )
	}

	time.Sleep(1000*1000*1000*1000)
}

