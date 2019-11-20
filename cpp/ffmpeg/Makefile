FFMPEG=/usr/local/Cellar/ffmpeg/4.1.3/lib
LIBS=-lbz2 -framework CoreFoundation -framework CoreVideo -framework VideoDecodeAcceleration -framework QuartzCore -lz -liconv -llzma  $(FFMPEG)/libavcodec.a $(FFMPEG)/libavutil.a $(FFMPEG)/libswscale.a $(FFMPEG)/libswresample.a -framework AudioToolbox -framework CoreMedia -laom -lopencore-amrnb -lopencore-amrwb -framework VideoToolbox -lmp3lame -lopenjp2 -lopus -lsoxr -lsnappy -lspeex -ltheora -lvorbis -lvorbisenc -lvpx -lx264 -lx265 -lxvidcore



enc: enc.cpp Makefile cumino.cpp
	g++  -std=c++11 -o enc -I$(FFMPEG)/include enc.cpp cumino.cpp $(LIBS)

cumino.o : cumino.cpp cumino.h
	g++ -c cumino.cpp

clean:
	rm enc *.o
