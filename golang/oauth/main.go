package main

import (
	"fmt";
	"http";
	"flag";
	"runtime";
	"io";
	"log";
)
	

// twitterの設定
var twitter_callback_url = "http://my.url/callback/twitter"
var twitter_consumer_key = "Vj1kPZIdQWvwjsanAzyxCA"
var twitter_consumer_secret = "G3pi811H2VaQbEipRZF9XWKA8Miz4Ubx9lqMesLyzsQ"
var twitter_client = NewTwitterClient(twitter_consumer_key, twitter_consumer_secret, "authenticate" )

// アプリの設定
var addr = flag.String( "addr", ":9988",  "http service address" )


type AuthClient struct {
	consumer_key string;
	consumer_secret string;
	request_url string; // where to request tokens
	access_url string; // where to exchange access tokens
	authorization_url string; // where to authorize
}


func NewTwitterClient( consumer_key string, consumer_secret string, authorization_type string) *AuthClient {
	return NewAuthClient( consumer_key, consumer_secret, "http://twitter.com/oauth/request_token", "http://twitter.com/oauth/access_token", "http://twitter.com/oauth/"+authorization_type);
}

func NewAuthClient( consumer_key string, consumer_secret string, request_url string, access_url string, authorization_url string) *AuthClient {
	c := new(AuthClient);
	c.consumer_key = consumer_key;
	c.consumer_secret = consumer_secret;
	c.request_url = request_url;
	c.access_url = access_url;
	c.authorization_url = authorization_url;
	return c;
}



func default_handler( c *http.Conn, req *http.Request ) {
	io.WriteString( c, "hello" )
}


func main() {
	fmt.Printf( "main start\n")

	flag.Parse()
	runtime.GOMAXPROCS(1)
	http.Handle( "/", http.HandlerFunc( default_handler  ) )

	err := http.ListenAndServe( *addr, nil )
	if err != nil {
		log.Exit( "ListenAndServe:", err )
	}
	
	
}
