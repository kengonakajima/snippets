package main

import (
	"http"
	"time"
//	"io"
	"websocket"
	"fmt"
)

// Echo the data received on the Web Socket.
func EchoServer(ws *websocket.Conn) {
	var buf = make([]byte,1024)

	for {
		time.Sleep(1*1000*1000)
		nr, err := ws.Read(buf);
		if( nr == 0 || err != nil ){
			continue;
		}

		s := string(buf[0:nr])
		fmt.Printf("data:'%s'\n" , s )
		ws.Write(buf[0:nr]);

		

	} 
}

func main() {
	http.Handle("/echo", websocket.Handler(EchoServer));
	err := http.ListenAndServe(":8080", nil);
	if err != nil {
		panic("ListenAndServe: " + err.String())
	}
}