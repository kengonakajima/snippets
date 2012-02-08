
P=$(shell pwd)
SRCSRC=a.c b.c c.c
SRC=$(SRCSRC:%.c=$(P)/%.o)

OBJ=$(SRC:.c=.o)
CC=gcc


all : $(OBJ)
	echo $(SRC)

