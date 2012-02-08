package main
import (
	"fmt";
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

	fmt.Printf("%d\n", runtime.GOMAXPROCS(0) )
	n := 1000
    m := 10000
	mmm = make( map[int]int, m )
	
	for i := 0; i < m; i++ {
		mmm[i] = 0
	}
	
	for i := 0; i < n; i++ {
		proc(m)
	}


	for i := 0; i < m; i++ {
//		fmt.Printf("%d ", mmm[i] )
		if mmm[i] != n {
			fmt.Printf("fail\n")
			return
		}
	}
	fmt.Printf("ok\n")
	
}

