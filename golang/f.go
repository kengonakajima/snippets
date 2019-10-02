package main
import(
	"fmt"
//	"bufio"
//	"io/ioutil"
	"os"
)

func check(e error) {
    if e != nil {
        panic(e)
    }
}

func main() {
	f,err :=os.Create("hoge_go.txt")
	check(err)

//	d2 := []byte{115, 111, 109, 101, 10}
	for i := 0; i < 1000000 ; i++ {
		s := fmt.Sprintf("hoge:%d\n",i)
		f.Write([]byte(s))
		f.Sync()
	}
}