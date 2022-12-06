OSSL=/opt/homebrew/Cellar/openssl@1.1/1.1.1s/lib/
SSH2=/opt/homebrew/Cellar/libssh2/1.10.0/lib/
CC = gcc
CFLAGS = -g -mmacosx-version-min=12.0  -g
LDFLAGS = $(OSSL)/libssl.a $(OSSL)/libcrypto.a $(SSH2)/libssh2.a libcurl.a -lz -lldap -framework SystemConfiguration -framework CoreFoundation -g

all: hoge fuga

hoge: hoge.c
	$(CC) $(LDFLAGS) -o hoge hoge.c

fuga: fuga.c
	$(CC) $(LDFLAGS) -o fuga fuga.c



clean:
	rm -f $(PROGRAM) $(OBJECTS)


