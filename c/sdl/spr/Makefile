# Makefile to build the SDL tests

srcdir  = .

CC      = gcc
EXE	= 
CFLAGS  = -g -O2 -I/usr/local/include/SDL -D_GNU_SOURCE=1 -D_THREAD_SAFE -DHAVE_OPENGL
LIBS	=  -L/usr/local/lib -lSDLmain -lSDL -Wl,-framework,Cocoa

TARGETS = testsprite

all: $(TARGETS)


testsprite: $(srcdir)/testsprite.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS) 


clean:
	rm -f $(TARGETS)

distclean: clean
	rm -f Makefile
	rm -f config.status config.cache config.log
	rm -rf $(srcdir)/autom4te*
