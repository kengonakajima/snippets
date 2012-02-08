package main
import(
//	"fmt"

)
func f(hideErrors bool) {
	defer func() {
		if x := recover(); x != nil {
			println("panicking with value", x)
			if !hideErrors {
				panic(x)  // go back to panicking
			}
		}
		println("function returns normally") // executes only when hideErrors==true
	}()
	println("before")
	p()
	println("after")	// never executes
}

func p() {
	panic(3)
}

type Player struct {
	id int
	name string
	next *Player

}

var cnt int
func g() {

	defer func() {
		x := recover()
		if x != nil {
			cnt++
//			println( "panicking with value",x)
		}
	}()
	q()
}
func q(){
	b := 2
	var a [10]int
	a[b]=1
}

func main(){
	for i :=0;i<1000000;i++ {
		g()
	}
	println( "panic cnt:", cnt )
}