all: a.out

a.out : hoge.cpp lodepng.cpp cumino.cpp snappy/snappy-c.cc snappy/snappy.cc snappy/snappy-sinksource.cc lz4/lz4.c lz4/lz4hc.c
	g++ -O3 hoge.cpp lodepng.cpp cumino.cpp snappy/snappy.cc snappy/snappy-c.cc  snappy/snappy-sinksource.cc lz4/lz4.c lz4/lz4hc.c -lz

clean :
	rm a.out
