package main

import (

	)

var ch chan int

func shared_goro () {  // shared by many worker goroutines 
	for {
		i := <- ch
		ch <- i*2
	}
}

func worker_goro ( id int ) { 

	ch <- id

	x := <- ch

	if x != id {
		panic("fatal")
	}
}

func main() {

	ch = make(chan int)

	go shared_goro()

	go worker_goro(1)
	go worker_goro(2)
	go worker_goro(3)
	

}

// チャネルのバッファが空かない限りは次の入力は待たされ、実行できない。
// worker1が書いて、それが実行終わるまでは他は待たされる。
// なので、workerが1回書いて1階よむ、sharedが1階よんで1階各　というのをやっているかぎり、デッドロックしない。
