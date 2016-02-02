LIBS=libavcodec.a libavutil.a libavformat.a  -lbz2 -framework CoreFoundation -framework CoreVideo -framework VideoDecodeAcceleration -framework QuartzCore -lz -liconv -llzma  libswscale.a libswresample.a cumino.o 

CFLAGS=-I/usr/local/include/SDL -I/Users/ringo/tmp -I/Users/ringo/tmp/FFmpeg -D__STDC_CONSTANT_MACROS

all: t1 t2 t3 t4 t5

t1: tutorial01.cpp cumino.o cumino.h
	g++ -o t1 tutorial01.cpp $(LIBS)
t2: tutorial02.cpp cumino.o cumino.h
	g++ -o t2 tutorial02.cpp $(LIBS) $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit
t3: tutorial03.c 
	gcc -o t3 tutorial03.c $(LIBS) $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit
t4: tutorial04.c
	gcc -o t4 tutorial04.c $(LIBS) $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit
t5: tutorial05.c
	gcc -o t5 tutorial05.c $(LIBS) $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit

cumino.o : cumino.cpp cumino.h
	g++ -c cumino.cpp
