all: aho.dylib a.out

aho.dylib : aho.c
	gcc -dynamiclib -o aho.dylib aho.c

a.out : use.c
	gcc use.c -lm aho.dylib
