

SRC=mm.c nakajima.c sano.c aho.c


OBJ=$(SRC:.c=.o)





snet.a : $(OBJ)
	rm -f snet.a
	ar rc snet.a $(OBJ)
	ranlib snet.a



