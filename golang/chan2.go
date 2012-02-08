package main

import (
	"fmt";
	"os";
	"time";

)

var filename = "tmpfile"

// groutine body
func w( v string) {
	fmt.Printf("start %s\n",v)
	file, _ := os.Open( filename, os.O_RDWR, 0644 )

	for i:=0; i<10000; i++ {
		time.Sleep( 100*1000 )
		fmt.Fprintf( file, "%s", v )
	}
	file.Close()
	fmt.Printf("end %s\n",v)
}

func main() {
	var v = []string{ ".",",",":","/","-", "$","#","!","<",">" }
	for i:= 0; i < 10; i++ {
		go w( v[i] )
	}
	time.Sleep(7*1000*1000*1000)
	fmt.Printf("main end\n")
}
