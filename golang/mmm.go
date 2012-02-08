package main
import (
	"fmt";
)

func main(){

	var m map[int]int
	var mp map[int]*int

	m = make( map[int]int,10 )
	mp = make( map[int]*int,10 )
	var ii int
	for i :=0; i< 1000;i++ {
		m[i]=i
		mp[i]=&ii
	}
	fmt.Printf("%d\n",len(m))
	fmt.Printf("%#v\n",mp[40495])
}
	