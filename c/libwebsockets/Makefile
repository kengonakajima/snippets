OPENSSLDIR=/opt/homebrew/Cellar/openssl@1.1/1.1.1o
INCFLAGS=-I$(OPENSSLDIR)/include -I/Users/ringo/libwebsockets/include
LINKFLAGS=$(OPENSSLDIR)/lib/libssl.a $(OPENSSLDIR)/lib/libcrypto.a

all: test


test: minimal-ws-server-echo.c protocol_lws_minimal_server_echo.c
	gcc $(INCFLAGS) minimal-ws-server-echo.c protocol_lws_minimal_server_echo.c ~/libwebsockets/lib/libwebsockets.a $(LINKFLAGS) -o test  

clean:
	rm -f test
