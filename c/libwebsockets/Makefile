OPENSSLDIR=/opt/homebrew/Cellar/openssl@1.1/1.1.1o
INCFLAGS=-I$(OPENSSLDIR)/include -I/Users/ringo/libwebsockets/include
LINKFLAGS=$(OPENSSLDIR)/lib/libssl.a $(OPENSSLDIR)/lib/libcrypto.a

all: test


test: main.cpp ws.cpp ws.h
	g++ $(INCFLAGS) -g main.cpp ws.cpp cumino.cpp ~/libwebsockets/lib/libwebsockets.a $(LINKFLAGS) -o test  

clean:
	rm -f test
