CC = gcc
CFLAGS = -c -O -W -Wall -Wstrict-prototypes -Wmissing-prototypes
MODFLAGS = -DMODULE -D__KERNEL__

all: aho.o

aho.o : aho.c
	$(CC) $(CFLAGS) $(MODFLAGS) aho.c
