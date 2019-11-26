#FFMPEG=/usr/local/Cellar/ffmpeg/4.2.1_2/lib
FFMPEG=./ffmpegmac/4.2
LIBS=-lbz2 -framework CoreFoundation -framework CoreVideo -framework VideoDecodeAcceleration -framework QuartzCore -lz -liconv -llzma  $(FFMPEG)/libavcodec.a $(FFMPEG)/libavformat.a $(FFMPEG)/libavutil.a $(FFMPEG)/libswscale.a $(FFMPEG)/libswresample.a -framework AudioToolbox -framework CoreMedia -laom -lopencore-amrnb -lopencore-amrwb -framework VideoToolbox -lmp3lame -lopenjp2 -lopus -lsoxr -lsnappy -lspeex -ltheora -lvorbis -lvorbisenc -lvpx -lx264 -lx265 -lxvidcore


all: enc audio astream

enc: enc.cpp Makefile cumino.cpp
	g++ -std=c++11 -g -o enc -I$(FFMPEG)/include enc.cpp cumino.cpp $(LIBS)

audio: audio.cpp Makefile cumino.cpp
	g++ -std=c++11 -g -o audio -I$(FFMPEG)/include audio.cpp cumino.cpp $(LIBS)

astream: astream.cpp Makefile cumino.cpp
	g++ -std=c++11 -g -o astream -I$(FFMPEG)/include astream.cpp cumino.cpp $(LIBS)

cumino.o : cumino.cpp cumino.h
	g++ -c cumino.cpp

clean:
	rm enc *.o
