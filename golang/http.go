package main

import (
	"http"
	"fmt"
	"io"
	"log"
	"runtime"
	"time"
)

// hello world, the web server
func HelloServer(w http.ResponseWriter, req *http.Request) {
	s := fmt.Sprintf( "{'now':%d}\n", time.Seconds() )
	io.WriteString(w, s );
}

func main() {
	runtime.GOMAXPROCS(1) // mapのmemcacheに必須の設定

	fmt.Print( "start\n");
	http.HandleFunc("/", HelloServer)
	err := http.ListenAndServe(":12345", nil)
	if err != nil {
		log.Exit("ListenAndServe: ", err.String())
	}
}


