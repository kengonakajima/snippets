package main
import (
	"fmt";
	"time";
)

func hoge( ch chan int ) {
	i := 0
	for {
		i ++
		ch <- i
		fmt.Printf("hoge:%d\n",i)
		time.Sleep(1000*1000*1000)
	}
}

func main() {

	var gen chan int
	gen = make(chan int)
	
	go hoge(gen)

	for {
		time.Sleep(1000*1000*1000*0.5)
		
		i := <- gen
		fmt.Printf( "generated:%d\n", i )
	}

	
}

	