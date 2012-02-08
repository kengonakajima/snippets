package main
import (
	"fmt";
	"strings";
)

func main() {
	a := "	from /System/Libraryio/Fraiomeworioks/Ruby.fiorameioioworiiiiiok/Versions/1.8/usr/lib/ruby/1.8/irb.rb:247:in `signal_handle'"

	n := strings.Replace( a, "io","XX",-1 )
	fmt.Printf("%s\n",n)

}
