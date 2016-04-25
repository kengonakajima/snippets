CFLAGS=-I/usr/local/include
LIBFLAGS=-I/usr/local/lib
LIBS=/usr/local/lib/libjpeg.a

all: a.out

a.out : hoge.cpp cumino.cpp
	g++ $(CFLAGS) -O3 hoge.cpp cumino.cpp $(LIBFLAGS) $(LIBS)

clean :
	rm a.out
