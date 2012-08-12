ISPC=/Users/ringo/bin/ispc

all: sumcpp sumispc

sumcpp: sum.cpp
	g++ sum.cpp -o sumcpp

sumispc: sumispc.cpp sum.ispc
	$(ISPC) -O2 --arch=x86-64 --target=sse4 -h sum_ispc.h -o sum_ispc.o sum.ispc 
	g++ sumispc.cpp -o sumispc sum_ispc.o

clean:
	rm sumispc sumcpp
