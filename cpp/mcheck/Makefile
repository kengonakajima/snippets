# yum install glibc-utils
test:
	g++ -g hoge.cpp
	MALLOC_TRACE=./hoge.trace ./a.out
	mtrace ./a.out ./hoge.trace

