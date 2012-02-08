package main

import (
	"fmt";
	)


type Player struct {
	name string;
	hp int;
	
}

func main(){

	pp := Player{ name:"ringo", hp:100 }
	fmt.Printf( "%T\n", pp )
	fmt.Printf( "%T\n", pp.name )
	fmt.Printf( "%s\n", pp.name )
	
	ary := []string{ "a", "b", "adsf" }
	players := []Player {  Player{ "ringo", 100 }, Player { "hio", 200 } }

	for _,k := range ary {
		fmt.Printf( "%s\n", k )
	}
	for _, p := range players {
		fmt.Printf( "%#v\n", p )
	}

	m := make(map[int]int,100)
	m[100] = 1
	m[1000] = 2
	m[10000] = 3

	for _, hoge := range m {
		fmt.Printf( "%d\n", hoge )
	}
	
	
}
