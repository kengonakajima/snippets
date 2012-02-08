UNAME=$(shell uname)

LIBUV=./deps/libuv/uv.a
CFLAGS=

all: $(UNAME)

Linux: linsv linws libluvwrap.so

Darwin: sv ws libluvwrap.dylib

sv: sv.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include sv.c $(LIBUV)   -o sv 

ws: ws.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/http-parser -Ideps/libuv/include  ws.c deps/http-parser/http_parser.o $(LIBUV) -o ws


linsv: sv.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include sv.c $(LIBUV)   -o sv -lpthread -lrt -lm

linws: ws.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/http-parser -Ideps/libuv/include  ws.c deps/http-parser/http_parser.o $(LIBUV) -o ws -lpthread -lrt -lm

libluvwrap.dylib: luvwrap.c
#	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include -undefined dynamic_lookup -dynamiclib  -o libluvwrap.dylib luvwrap.c
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include -dynamiclib  $(LIBUV) -o libluvwrap.dylib luvwrap.c

libluvwrap.so: luvwrap.c
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include -shared -fPIC -Wl,-soname=libluvwrap.so luvwrap.c $(LIBUV) -o libluvwrap.so   -lrt -lm
	nm libluvwrap.so |grep ev_default_loop_ptr

clean:
	rm -f libluv*.so libluv*.dylib