LIBS=libavcodec.a libavutil.a libavformat.a  -lbz2 -framework CoreFoundation -framework CoreVideo -framework VideoDecodeAcceleration -framework QuartzCore -lz -liconv -llzma  libswscale.a libswresample.a cumino.o 

CFLAGS=-I/usr/local/include/SDL

all: t1 t2 t3

t1: tutorial01.cpp cumino.o cumino.h
	g++ -o t1 tutorial01.cpp $(LIBS)
t2: tutorial02.cpp cumino.o cumino.h
	g++ -o t2 tutorial02.cpp $(LIBS) $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit
t3: tutorial03.cpp cumino.o cumino.h
	g++ -o t3 tutorial03.cpp $(LIBS) $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit

cumino.o : cumino.cpp cumino.h
	g++ -c cumino.cpp
