OPENSSL=/usr/local/Cellar/openssl@1.1/1.1.1d
OPENSSLINC=-I$(OPENSSL)/include
OPENSSLLIBS=$(OPENSSL)/lib/libcrypto.a $(OPENSSL)/lib/libssl.a

mac:
	gcc $(OPENSSLINC) v.c
	./a.out

