package main

import (
	"fmt";
	"container/list";
)


type X struct {
	id int


}
func (x *X)Value() (int){
	return x.id
}

func main() {
	l := list.New()

	fmt.Printf("%d\n", l.Len())

	y := X { id : 20 }
	
	l.PushFront( &y )

	fmt.Printf("%d\n", l.Len())
	
	x := X { id : 10 }

	l.PushFront( &x )
	l.PushFront( &x )
	l.PushFront( &x )	

	fmt.Printf("%d\n", l.Len())

	for e := range l.Iter() {
		fmt.Printf("%d\n",e.(*X).id )
	}

	var m map[int]*list.List

	m = make( map[int]*list.List )
	m[100] = l

	var ll *list.List
	for i:=0;i<100000;i++ {
		ll = m[100]
		for e := range ll.Iter() {
			if e.(*X).id == 8494 {
				fmt.Printf("%d\n",e.(*X).id )
			}
		}
	}
}
