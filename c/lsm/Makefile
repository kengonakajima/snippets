CC=gcc
PROG=lssendmesg
CFLAGS= -O -Wall -s
LFLAGS= -lsocket -lnsl

SRC = main.c
OBJ = $(SRC:.c=.o)

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJ) $(LFLAGS)

depend:
	make clean
	makedepend $(SRC)

clean:
	rm -f $(OBJ)
# DO NOT DELETE THIS LINE -- make depend depends on it.

main.o: /usr/include/stdio.h /usr/include/sys/feature_tests.h
main.o: /usr/include/sys/va_list.h /usr/include/stdlib.h
main.o: /usr/include/stdarg.h /usr/include/varargs.h /usr/include/strings.h
main.o: /usr/include/macros.h /usr/include/sys/types.h
main.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
main.o: /usr/include/sys/stat.h /usr/include/sys/time.h /usr/include/ulimit.h
main.o: /usr/include/sys/ulimit.h /usr/include/unistd.h
main.o: /usr/include/sys/unistd.h /usr/include/sys/socket.h
main.o: /usr/include/sys/netconfig.h /usr/include/netinet/in.h
main.o: /usr/include/sys/stream.h /usr/include/sys/vnode.h
main.o: /usr/include/sys/t_lock.h /usr/include/sys/machlock.h
main.o: /usr/include/sys/dki_lkinfo.h /usr/include/sys/dl.h
main.o: /usr/include/sys/sleepq.h /usr/include/sys/turnstile.h
main.o: /usr/include/sys/param.h /usr/include/sys/pirec.h
main.o: /usr/include/sys/mutex.h /usr/include/sys/rwlock.h
main.o: /usr/include/sys/semaphore.h /usr/include/sys/condvar.h
main.o: /usr/include/sys/cred.h /usr/include/sys/uio.h
main.o: /usr/include/vm/seg_enum.h /usr/include/sys/poll.h
main.o: /usr/include/sys/strmdep.h /usr/include/sys/byteorder.h
main.o: /usr/include/netdb.h
