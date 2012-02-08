package main
import (
	"fmt";
)

type Player struct {
	id int64
	name string
}

func main() {

	p := Player{ id:10, name:"ringo" }

	var m map[int64]*Player

	m = make(map[int64]*Player, 100 )
	
	m[p.id] = &p

	fmt.Printf("hoge:%d %s\n", m[10].id, m[10].name )
	
}


