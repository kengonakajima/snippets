SHELL = /bin/sh

#### Start of system configuration section. ####

srcdir = /tmp/ruby-1.6.0
topdir = /usr/local/lib/ruby/1.6/i686-linux
hdrdir = /usr/local/lib/ruby/1.6/i686-linux

CC = gcc

CFLAGS   = -fPIC -g -O2 -g -I. -Wall
CPPFLAGS = -I$(hdrdir) -I/usr/local/include  
CXXFLAGS = $(CFLAGS)
DLDFLAGS =  
LDSHARED = gcc -shared 

RUBY_INSTALL_NAME = ruby
RUBY_SO_NAME = 

DESTDIR =
prefix = $(DESTDIR)/usr/local
exec_prefix = $(DESTDIR)/usr/local
libdir = $(DESTDIR)/usr/local/lib/ruby/1.6
archdir = $(DESTDIR)/usr/local/lib/ruby/1.6/i686-linux
sitelibdir = $(DESTDIR)/usr/local/lib/ruby/site_ruby/1.6
sitearchdir = $(DESTDIR)/usr/local/lib/ruby/site_ruby/1.6/i686-linux

#### End of system configuration section. ####

LOCAL_LIBS =  
LIBS = -lc
OBJS = hoge.o

TARGET = hoge
DLLIB = $(TARGET).so

RUBY = ruby
RM = $(RUBY) -r ftools -e 'File::rm_f(*Dir[ARGV.join(" ")])'

EXEEXT = 

all:		$(DLLIB)

clean:;		@$(RM) *.o *.so *.sl *.a $(DLLIB)
		@$(RM) $(TARGET).lib $(TARGET).exp $(TARGET).ilk *.pdb

distclean:	clean
		@$(RM) Makefile extconf.h conftest.*
		@$(RM) core ruby$(EXEEXT) *~

realclean:	distclean

install:	$(archdir)/$(DLLIB)

site-install:	$(sitearchdir)/$(DLLIB)

$(archdir)/$(DLLIB): $(DLLIB)
	@$(RUBY) -r ftools -e 'File::makedirs(*ARGV)' $(libdir) $(archdir)
	@$(RUBY) -r ftools -e 'File::install(ARGV[0], ARGV[1], 0555, true)' $(DLLIB) $(archdir)/$(DLLIB)

$(sitearchdir)/$(DLLIB): $(DLLIB)
	@$(RUBY) -r ftools -e 'File::makedirs(*ARGV)' $(libdir) $(sitearchdir)
	@$(RUBY) -r ftools -e 'File::install(ARGV[0], ARGV[1], 0555, true)' $(DLLIB) $(sitearchdir)/$(DLLIB)


.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
$(DLLIB): $(OBJS)
	$(LDSHARED) $(DLDFLAGS) -o $(DLLIB) $(OBJS) $(LIBS) $(LOCAL_LIBS)
