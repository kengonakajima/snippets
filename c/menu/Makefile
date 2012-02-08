
CC=gcc
RM=rm -f

MV=mv

SHELL=/bin/sh

CFLAGS=-Wall -O -g
A=-I/usr/include/ncurses
PROG=menu

SRC=main.c
OBJ=$(SRC:.c=.o)
LDFLAGS=-lncurses -ltermcap

all: $(PROG)
$(PROG):$(OBJ)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~
