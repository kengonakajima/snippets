all : cli

cli : cli.c udpc.c
	gcc -o cli -lnsl -lsocket udpc.c cli.c

serv : serv.c udpc.c
	gcc -o serv -lnsl -lsocket serv.c udpc.c
