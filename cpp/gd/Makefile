all: a.out

cumino.o : cumino.cpp
	g++ cumino.cpp -c

a.out: hoge.cpp cumino.o
	g++ hoge.cpp cumino.o -o a.out -lgd

