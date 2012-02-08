package main
import (
	"fmt";
	"crypto/hmac";
)

func main() {
//	myhash := hmac.NewSHA1( []byte(key));
//	myhash.Write( []byte(m));	
//	signature := bytes.TrimSpace(myhash.Sum());
//	digest := make([]byte, base64.StdEncoding.EncodedLen(len(signature)));
//	base64.StdEncoding.Encode(digest, signature);
//	digest_str := strings.TrimSpace(bytes.NewBuffer(digest).String());

	h := hmac.NewSHA1( []byte( "aho" ))
	h.Write( []byte( "AAAAAAAAAAAAAAAAAAAAAAA"))
	s := h.Sum()
	var v uint
	b := []byte(s)
	v = uint( b[0] ) + uint(b[1]) * 256 + uint(b[2])*65536 + uint(b[3])*(65536*256)
	fmt.Printf("%#v %#v\n", v, b)

	
}
	
	
	