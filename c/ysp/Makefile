#
# Makefile for toybox city server
#
# created by ringo@vce-lab.net 2001 sep
#
#


all :
	flex aho.lex
	bison -d aho.y
	gcc -pg -O2 lex.yy.c aho.tab.c


