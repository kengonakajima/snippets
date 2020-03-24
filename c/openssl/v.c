#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/rand.h> // RAND_bytes
#include <openssl/ossl_typ.h> // internal

#include <stdio.h>

int main() {
    printf("%lx\n",OPENSSL_VERSION_NUMBER);
    printf("%d\n",(OPENSSL_VERSION_NUMBER & 0x10101000) == 0x10101000 );
    printf("%d\n",(OPENSSL_VERSION_NUMBER & 0x10002000) == 0x10002000 );
}
