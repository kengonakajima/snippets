p : linux.o proxy.o
	gcc  -o p linux.o proxy.o 

linux.o : linux.c
	gcc -g -c -O2 -Wall -g linux.c


proxy.o : proxy.c
	gcc -g -c -O2 -Wall -g proxy.c

#lex.yy.o : p.lex
#	flex p.lex
#	gcc -c -O2 lex.yy.c



clean :
	rm a.out *.o

