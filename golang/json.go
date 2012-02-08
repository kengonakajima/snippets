package main

import (
    "json"
	"fmt"
)

type Data struct {
	x int
	y string
	s []string
}

func main() {

	var data Data


	data.x = 10
	data.y = "aho"
	data.s = []string { "aho", "hoge", "fuga" }

	b, _ := json.Marshal(&data)


	js := string(b)
	
	fmt.Printf( "marshaled:%s\n" , js )


	var data2 Data
    err := json.Unmarshal( b, &data2 )

	fmt.Printf( "err:%#v x:%#v\n", err, data.x )



}