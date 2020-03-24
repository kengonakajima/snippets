#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/rand.h> // RAND_bytes
#include <openssl/ossl_typ.h> // internal

#include <stdio.h>

#define OPENSSL_1_1_0  (OPENSSL_VERSION_NUMBER & 0x10101000) == 0x10101000 )
#define OPENSSL_1_0_2  (OPENSSL_VERSION_NUMBER & 0x10000200) == 0x10002000 )

int main() {
    printf("%lx\n",OPENSSL_VERSION_NUMBER);
    printf("%d\n", OPENSSL_1_1_0 );
    printf("%d\n", OPENSSL_1_0_2 );
}
