UNAME=$(shell uname)

LIBUV=./libuv/.libs/libuv.a

CFLAGS=-Ilibuv/include -Wall -pg -g
#CFLAGS=-Ilibuv/include -Wall -O3

ifeq ($(UNAME),Darwin)
LIBFLAGS=
else
LIBFLAGS=-lpthread
endif




udp: udp.c $(LIBUV)
	gcc $(CFLAGS) udp.c $(LIBUV) $(LIBFLAGS) -o udp

clean:
	rm -f *.o sv cl
