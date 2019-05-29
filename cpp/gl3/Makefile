FFMPEG=/usr/local/Cellar/ffmpeg/4.1.3/
GLFW=../../../ssg/moyai/glfw-3.2
MRS=../../../udp
OPENSSL=../../../udp/deps/OpenSSL_1_0_2-stable-macos10.14

CFLAGS=-std=c++0x -I$(GLFW)/include -I. -Wno-deprecated-declarations -Iglm-0.9.7.1 -O3 -m64 -I$(MRS)



OSX_TARGET=10.14
OSX_TARGET_FLAG= #-mmacosx-version-min=$(OSX_TARGET)

CLILIBFLAGS=-lavcodec -framework Cocoa -framework IOKit -framework OpenGL -framework CoreFoundation -framework CoreVideo -m64 -L/usr/local/lib -framework OpenAL $(OSX_TARGET_FLAG) $(MRS)/libmrs.a $(OPENSSL)/libcrypto.a $(OPENSSL)/libssl.a $(MRS)/libmru.a $(MRS)/deps/enet-macos10.14/libenet.a $(MRS)/deps/libuv-macos10.14/libuv.a


a.out: main.o cumino.o
	g++  $(CFLAGS) -o a.out main.o cumino.o $(GLFW)/src/libglfw3.a $(CLILIBFLAGS)

main.o : main.cpp
	g++  $(CFLAGS) -c main.cpp

cumino.o : cumino.cpp
	g++ $(CFLAGS) -c cumino.cpp 

clean:
	rm a.out cumino.o main.o
