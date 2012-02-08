package main
import (
	"fmt";
)

type Hoge interface {
	Read() int
	Write(int )int
}

type HT struct {
}

func ( self HT )Read() int {
	return 2
}
func ( self HT )Write(int) int {
	return 3
}


type Aho struct {
	a map[string]Hoge
}



func ( self Aho ) cons(h Hoge) (int) {
	self.a = make( map[string]Hoge,100)
	self.a["aho"] = h
	return h.Read()
}

func main(){

	var aho Aho
	
	fmt.Printf("start %#v\n",aho)

	var h HT

	fmt.Printf("%d\n", aho.cons(h ))
	
}
