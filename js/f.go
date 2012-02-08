package main
import(
	"fmt";
)

func fff( a int ) int {	return a + 2 }
func ggg( a int ) int {
	var h = make([]int,10)
	h[0] = a
	return h[0]
}

func main() {
	var t float64 = 0
	for i :=0; i < 100000000; i++ {
		t += float64(fff(i))
	}
	fmt.Printf( "ttt:%f\n", t )
}