package main
import (
	"fmt";
)


func main() {
	var s = "askdjfhalksdjhalksjhdflakjdf"
	var t = "日本は全国的に晴れ"

	fmt.Printf("%#v\n", s[0] )
	fmt.Printf("%#v\n", t[0] )
	fmt.Printf("%#v\n", t[1] )
	fmt.Printf("%#v\n", t[2] )
	fmt.Printf("%d\n", len(t) )

	fmt.Printf("%#v\n", 'x' )
	fmt.Printf("%#v\n", byte('x') )	
	
}
