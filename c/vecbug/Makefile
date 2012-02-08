all: sv cl

sv: sv.c
	gcc -I/home/ringo/vce/lib/ sv.c -o sv /home/ringo/vce/lib/libvce.a -Wall

cl: cl.c
	gcc -I/home/ringo/vce/lib/ cl.c -o cl /home/ringo/vce/lib/libvce.a -Wall
start_swp:
	/home/ringo/vce/lib/swp swp.conf swtable.conf
