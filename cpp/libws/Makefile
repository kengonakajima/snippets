OPENSSL=/usr/local/Cellar/openssl@1.1/1.1.1b
CFLAGS=-Ilibwebsockets/include -I$(OPENSSL)/include


a.out: main.cpp
	g++ main.cpp $(CFLAGS) libwebsockets/lib/libwebsockets.a $(OPENSSL)/lib/libssl.a $(OPENSSL)/lib/libcrypto.a


