mac:
#	gcc -I /usr/local/Cellar/openssl@1.1/1.1.1d/include hoge.c /usr/local/Cellar/openssl@1.1/1.1.1d/lib/libcrypto.a /usr/local/Cellar/openssl@1.1/1.1.1d/lib/libssl.a
	gcc -g -I /Users/ringo/openssl/include hoge.c /Users/ringo/openssl/libcrypto.a /Users/ringo/openssl/libssl.a
	./a.out cert.pem key.pem DST_Root_CA_X3.pem

linux: 
	gcc -I /usr/local/Cellar/openssl@1.1/1.1.1d/include hoge.c -lcrypto -lssl


