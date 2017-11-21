CFLAGS=-I../../../libuv/include
LIBS=../../../libuv/.libs/libuv.a

all: sv cl

cl: cl.c
	gcc $(CFLAGS) $(LIBS) cl.c -o cl

sv: sv.c
	gcc $(CFLAGS) $(LIBS) sv.c -o sv 
