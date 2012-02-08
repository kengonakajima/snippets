all : cli serv

cli : cli.c rc.c
	gcc -O2 -lnsl -lsocket -g -o cli cli.c rc.c


serv : serv.c rc.c
	gcc -O2 -lnsl -lsocket -g -o serv serv.c rc.c

