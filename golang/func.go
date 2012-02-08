package main

import (
	"fmt";
	)


func aho( a int, f func() ) {
	fmt.Printf("%d\n",a )
	f()
}

func huga(){
	fmt.Printf("huga\n")
}

func main(){
	f := func() {
		fmt.Printf( "hello\n" )
		huga()
	}
	aho(10,f)
}
