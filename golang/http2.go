package main


import (
	"fmt";
	"http";
	"os";
	"io";
	"runtime";
	"log";
	"time";
)

func main() {
	runtime.GOMAXPROCS(1) // mapのmemcacheに必須の設定
	
	fmt.Fprintf( os.Stderr, "server start\n")

	go http1()
	go http2()

	time.Sleep(1000*1000*1000*1000)
}

func test1_handler(  c *http.Conn, req *http.Request ) {
	io.WriteString( c, "hello1" )	
}
func test2_handler(  c *http.Conn, req *http.Request ) {
	io.WriteString( c, "hello2" )		
}

func http1() {
	
	http.Handle( "/test", http.HandlerFunc( test1_handler  ) )

	err := http.ListenAndServe( ":8080", nil )
	if err != nil {
		log.Exit( "ListenAndServe:", err )
	}
}

func http2() {
	
	http.Handle( "/test", http.HandlerFunc( test2_handler  ) )

	err := http.ListenAndServe( ":8081", nil )
	if err != nil {
		log.Exit( "ListenAndServe:", err )
	}
}
