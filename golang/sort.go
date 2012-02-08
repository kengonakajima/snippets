package main

import (
	"fmt";
	"sort";
)


type A struct {
	id int
	name string
	hp int
}

type Sorter struct {
	ary []*A
}

func (a *Sorter) Len() int {
	return len(a.ary)
}
func (a *Sorter) Swap(i,j int ) {
	a.ary[i], a.ary[j] = a.ary[j], a.ary[i]
}

func (a *Sorter) Less(i,j int ) bool{
	return a.ary[i].hp < a.ary[j].hp
}

func main() {
	
	var s Sorter
	s.ary = make( []*A, 4 )
	s.ary[0] = &A{ 1,"r",3 }
	s.ary[1] = &A{ 4,"r",8 }
	s.ary[2] = &A{ 2,"r",11 }
	s.ary[3] = &A{ 9,"r",4 }	
	sort.Sort(&s)

	fmt.Printf( "%d\n", s.ary[3].hp)
}
