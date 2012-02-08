// 大きめの2次元配列の要素を全部加算して合計をもとめると。。

package main

import (
	"fmt"
	"time"
	)

func main() {
	data := make( []int, 1024*1024 )
	t := time.Nanoseconds();
	for i := 0; i < 100 ; i ++ {
		doit(data);
	}
	e := time.Nanoseconds();
	fmt.Printf( "dt:%d ms\n", ( e - t ) / 1000 / 1000 );
}
func doit( data []int) (int) {
	
	tot := 0
	for y := 0; y < 1024; y++ {
		for x := 0; x < 1024; x ++ {
			data[y*1024+x] ++;
			tot += data[y*1024+x];
		}
	}
	return tot
}

