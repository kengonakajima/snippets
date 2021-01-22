NDK=/Users/ringo/android-ndk-r20
CLANG=$(NDK)/toolchains/llvm/prebuilt/darwin-x86_64/bin/aarch64-linux-android21-clang++
LD=$(NDK)/toolchains/llvm/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-ld    
AR=$(NDK)/toolchains/llvm/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-ar
NM=$(NDK)/toolchains/llvm/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-nm

ROOT=$(NDK)/platforms/android-21/arch-arm64

test:
	$(CLANG) -target arm-linux-androideabi-4.9 -shared -fpic hoge.cpp -o hoge.dylib --sysroot=$(ROOT)
	file hoge.dylib
#	$(CLANG) -target arm-linux-androideabi-4.9 main.cpp -c 
#	$(AR) r hoge.a hoge.o
#	$(NM) hoge.a
