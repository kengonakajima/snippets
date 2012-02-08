CC = gcc

all: doit

doit: mlc.o mltest.c mlc.h
	gcc -O2  -o doit mlc.o mltest.c



test: testit

testit: mlc.o mltcomp.c mlc.h
	gcc -O2 -lgmp -o testit mlc.o mltcomp.c



clean:
	rm *.o

bug: bugtest

bugtest: mltbug.c mlc.h
#	gcc -O2 -lgmp -o bugtest mltbug.c
	gcc -I./ -o bugtest ./libgmp.a mltbug.c 
