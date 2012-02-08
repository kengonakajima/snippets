package main
import(
	"fmt"
)
type Vector3 struct {
	x float32
	y float32
	z float32
}

func newVector3( x float32, y float32, z float32 ) *Vector3 {
	o := new(Vector3)
	o.x = x
	o.y = y
	o.z = z
	return o
}
func translateVector3( v *Vector3, arg *Vector3 ) *Vector3 {
	v.x += arg.x
	v.y += arg.y
	v.z += arg.z	
	return v
}
func main() {
	NLOOP := 10000
	NVEC := 10000
	var ary = make( []*Vector3, NVEC )
	var ary1 = make( []*Vector3, NVEC )
	for i:=0; i<NVEC; i++ {
		ary[i] = newVector3(0,0,0)
		ary1[i] = newVector3(1,1,1)
	}
	for i:=0; i<NLOOP;i++ {
		for j:=0; j<NVEC;j++ {
			translateVector3( ary[j], ary1[j] )
		}
	}
	v := ary[NVEC/2]
	fmt.Printf( "%f %f %f", v.x, v.y, v.z )
}