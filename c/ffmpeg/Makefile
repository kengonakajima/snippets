LIBS=libavcodec.a libavutil.a libavformat.a  -lbz2 -framework CoreFoundation -framework CoreVideo -framework VideoDecodeAcceleration -framework QuartzCore -lz -liconv -llzma  libswscale.a libswresample.a cumino.o 

CFLAGS=-I/usr/local/include/SDL -I/Users/ringo/tmp -I/Users/ringo/tmp/FFmpeg -D__STDC_CONSTANT_MACROS

all: t1 t2 t3 t4 t5 t6 ffplay

t1: tutorial01.cpp cumino.o cumino.h
	g++ -o t1 tutorial01.cpp $(LIBS)
t2: tutorial02.cpp cumino.o cumino.h
	g++ -o t2 tutorial02.cpp $(LIBS) $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit
t3: tutorial03.c 
	gcc -o t3 tutorial03.c $(LIBS) $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit
t4: tutorial04gh.c
	gcc -o t4 tutorial04gh.c $(LIBS) $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit
t5: tutorial05gh.c
	gcc -o t5 tutorial05gh.c $(LIBS) $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit
t6: tutorial06gh.c
	gcc -o t6 tutorial06gh.c $(LIBS) $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit
ffplay: ffplay.c
	gcc -o ffplay ffplay.c cmdutils.c $(LIBS) libavdevice.a libavfilter.a $(CFLAGS) -lSDL -lSDLmain -framework CoreGraphics -framework AppKit -framework CoreMedia -framework AVFoundation -framework QTKit

cumino.o : cumino.cpp cumino.h
	g++ -c cumino.cpp

clean:
	rm t1 t2 t3 t4 t5 t6 ffplay
