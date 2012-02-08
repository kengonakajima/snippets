all : cli serv tcli tserv ucli userv

serv : serv.c rp.c
	gcc -Wall -O -g -o serv -lnsl -lsocket serv.c rp.c
	rm setsendack.txt
	rm recvp.txt

cli : cli.c rp.c
	gcc -Wall -O -g -o cli -lnsl -lsocket cli.c rp.c

tcli : tcli.c
	gcc -O -g -o tcli -lnsl -lsocket tcli.c

tserv : tserv.c
	gcc -O -g -o tserv -lnsl -lsocket tserv.c

ucli : ucli.c
	gcc -O -g -o ucli -lnsl -lsocket ucli.c

userv : userv.c
	gcc -O -g -o userv -lnsl -lsocket userv.c


uclilinux : ucli.c
	gcc -O -g -o ucli ucli.c

uservlinux : userv.c
	gcc -O -g -o userv userv.c








uclifree : ucli.c
	gcc -O -g -o ucli ucli.c

uservfree : userv.c
	gcc -O -g -o userv userv.c


