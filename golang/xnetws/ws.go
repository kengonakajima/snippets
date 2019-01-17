package main
import(
//	"io"
	"fmt"
	"golang.org/x/net/websocket"
	"net/http"
)

func EchoHandler(ws *websocket.Conn) {
//	fmt.Printf("message");

	for {
		var msg string
		err := websocket.Message.Receive(ws,&msg)
		if err != nil {
			fmt.Printf("err\n");
			return
		}
		fmt.Printf("msg=%s\n",msg);
		vec := []byte(msg);
		fmt.Println(vec);
	}
//    io.Copy(ws, ws)
}
func BinaryHandler(ws *websocket.Conn) {
	var msg string
	for {
		websocket.JSON.Receive(ws,&msg)
		fmt.Printf("msg=%s\n",msg)
	}
}

func main() {
	http.Handle("/echo", websocket.Handler(EchoHandler))
	fmt.Printf("start\n");
	err := http.ListenAndServe(":12345",nil)
	if err!= nil {
		panic("errrorr:"+err.Error())
	}
}