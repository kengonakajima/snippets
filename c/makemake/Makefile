#
# created by mkmk.perl
#
	
CC=gcc
SRC=./aho.c ./hoge/a.c
OBJ=${SRC:.c=.o}
CFLAGS=-Wall -O -g -I. -I./hoge 
LIBS=
PROG=a.out
RM=/bin/rm
	
${PROG} : ${OBJ}
	${CC} ${CFLAGS} -o ${PROG} ${OBJ} ${LIBS}


clean :
	${RM} -f *.o ${PROG}


