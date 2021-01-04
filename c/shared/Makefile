test:
	gcc main.c hoge.dylib
	otool -L a.out
	./a.out


poo.dylib : hoge.c
	gcc -shared -fpic hoge.c -o hoge.dylib
	file hoge.dylib

