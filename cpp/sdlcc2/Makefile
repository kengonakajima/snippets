
all :
	bison -d obj.y
	flex obj.lex
	gcc -Wall -g aho.c lex.yy.c obj.tab.c

