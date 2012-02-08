


# only solaris
LIBOPT = -lnsl -lsocket

all : cl sv

cl : tsrpc.o cliapp.o tsrpc_client.o
	gcc ${LIBOPT} tsrpc_client.o tsrpc.o cliapp.o -o cl

sv : tsrpc.o tsrpc_server.o servapp.o
	gcc ${LIBOPT} tsrpc_server.o tsrpc.o servapp.o -o sv

servapp.o : servapp.c
	gcc -g -c servapp.c

tsrpc_server.o : tsrpc_server.c
	gcc -g -c tsrpc_server.c

cliapp.o : cliapp.c
	gcc -g -c cliapp.c

tsrpc.o : tsrpc.c
	gcc -g -c tsrpc.c

tsrpc_client.o : tsrpc_client.c
	gcc -g -c tsrpc_client.c

