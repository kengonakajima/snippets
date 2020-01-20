#include <openssl/pem.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <assert.h>
#include <openssl/ossl_typ.h>
#include <openssl/x509_vfy.h>
#include <openssl/x509.h>



double now() {
    struct timeval tmv;
    gettimeofday( &tmv, NULL );
    return tmv.tv_sec  + (double)(tmv.tv_usec) / 1000000.0f;
}


X509_STORE *setup_verify(const char *CAfile, const char *CApath, int noCAfile, int noCApath)
{
    X509_STORE *store = X509_STORE_new();
    X509_LOOKUP *lookup;

    if (store == NULL)
        goto end;

    if (CAfile != NULL || !noCAfile) {
        lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
        if (lookup == NULL)
            goto end;
        if (CAfile) {
            if (!X509_LOOKUP_load_file(lookup, CAfile, X509_FILETYPE_PEM)) {
                printf( "Error loading file %s\n", CAfile);
                goto end;
            }
        } else
            X509_LOOKUP_load_file(lookup, NULL, X509_FILETYPE_DEFAULT);
    }

    if (CApath != NULL || !noCApath) {
        lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
        if (lookup == NULL)
            goto end;
        if (CApath) {
            if (!X509_LOOKUP_add_dir(lookup, CApath, X509_FILETYPE_PEM)) {
                printf( "Error loading directory %s\n", CApath);
                goto end;
            }
        } else
            X509_LOOKUP_add_dir(lookup, NULL, X509_FILETYPE_DEFAULT);
    }

    ERR_clear_error();
    return store;
 end:
    X509_STORE_free(store);
    return NULL;
}


int main(int argc, char **argv) {
    char key[4096]={};
    int keysz=0;
    char cert[4096]={};
    int certsz=0;
    
    FILE *fp;
    fp=fopen(argv[1],"r");
    keysz=fread(key,1,sizeof(key),fp);
    fclose(fp);
    fp=fopen(argv[2],"r");
    certsz=fread(cert,1,sizeof(cert),fp);
    fclose(fp);
    printf("keysz:%d certsz:%d\n",keysz,certsz);

    BIO *bio_mem = BIO_new(BIO_s_mem());
    BIO_puts(bio_mem, cert);
    X509 *x = PEM_read_bio_X509(bio_mem,NULL,NULL,NULL);
    assert(x);
    EVP_PKEY *pkey = X509_get_pubkey(x);
    assert(pkey);
    int r=X509_verify(x,pkey);
    //    EVP_PKEY_free(pkey);
    //    BIO_free(bio_mem);
    //    X509_free(x);
    printf("x509_verify: r:%d\n",r);
    X509_STORE_CTX *ctx;
    ctx=X509_STORE_CTX_new();
    assert(ctx);
    //    X509_STORE *store = X509_STORE_new();
    X509_STORE *store = setup_verify( "/etc/ssl/certs/DST_Root_CA_X3.pem", NULL, 0,0);
    assert(store);
    r=X509_STORE_add_cert(store,x);
    if(r!=1) {
        printf("x509_store_add_cert failed\n" );
    }
    printf("x509_store_add_cert: r:%d\n",r);
    r=X509_STORE_CTX_init(ctx,store,x,NULL);
    if(r!=1) printf("x509_store_ctx_init failed");
    printf("x509_store_ctx_init: r:%d\n",r);
    r=X509_verify_cert(ctx);
    if(r == 1) {
        printf("Certificate verified ok\n");
    }else {
        printf("x509_verify_cert fail: %s\n", X509_verify_cert_error_string(X509_STORE_CTX_get_error(ctx)));
    }    
    return 0;
}

