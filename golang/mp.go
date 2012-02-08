// mempool

package main

import (
	"log"
	"time"
//	"fmt"
)

type Player struct {
	id [100]int
	next *Player
}

var freelist *Player

func allocP() ( *Player ){
	if freelist == nil {
//		fmt.Printf( "n")
		return new(Player)
	} else {
//		fmt.Printf("l")
		p := freelist
		freelist = freelist.next
		return p
	}
	return nil
}

func freeP( p *Player ){
//	fmt.Printf("f")
	p.next = freelist
	freelist = p
}

// new/makeの速度検証
func main() {
	k := 10
	n := 1000000
	p := make( []*Player, n )
	
	for j:=0;j<k;j++ {
		log.Printf( "start\n")
		t := time.Nanoseconds()
		for i:=0;i<n;i++ {
			p[i] = allocP()
		}
		for i:=0;i<n;i++ {
			p[i].id[99] = i
			p[i].id[0] = i
		}
		for i:=0;i<n;i++ {
			freeP(p[i])
		}
		e := time.Nanoseconds()
		log.Printf( "end: %d\n", ( e - t ) / 1000 / 1000 )
	}

	time.Sleep(1000*1000*1000*1000)
}

