all:
	llvm-gcc -emit-llvm -pg dhry.c -c -o dhry.ll
	lldis dhry.ll > dhry.ll.s
	gcc dhry.c -o dhry.gccout
