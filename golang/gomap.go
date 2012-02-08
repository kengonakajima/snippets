package main
import (
	"fmt";
	"time";
	"runtime";
)






var mmm map[int]int

func proc(loop int) {
	for i := 0; i < loop; i++ {
		var x = mmm[i]
		x ++
		mmm[i] = x
	}
}


func main() {

	fmt.Printf("%d\n", runtime.GOMAXPROCS(4) )
	
	n := 500
    m := 10000
	mmm = make( map[int]int, m )
	
	for i := 0; i < m; i++ {
		mmm[i] = 0
	}
	
	for i := 0; i < n; i++ {
		go proc(m)
	}
	time.Sleep(1000*1000*1000*3)

	for i := 0; i < m; i++ {
		fmt.Printf("%d\n", mmm[i] )
		if mmm[i] != n {
			fmt.Printf("fail\n")
			return
		}
	}
	fmt.Printf("ok\n")
	
}

