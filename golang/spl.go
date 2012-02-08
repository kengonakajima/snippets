package main

import (
	"fmt";
	"strings";
)



func main() {
	s := "http://aho:ghoge/fuga/muga"
	ary := strings.Split(s, "/", -1 )

	for i := range ary {
		fmt.Printf( "%s\n", ary[i] )
	}
}
