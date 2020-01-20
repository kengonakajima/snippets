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

int main() {
    char key[4096]={};
    int keysz=0;
    char cert[4096]={};
    int certsz=0;
    
    FILE *fp;
    fp=fopen("key.pem","r");
    keysz=fread(key,1,sizeof(key),fp);
    fclose(fp);
    fp=fopen("cert.pem","r");
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
    X509_STORE *store = X509_STORE_new();
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

/*
static bool ssl_set_certificate_data( SSL* ssl, const char* data ){
    bool result = false;
    BIO* bio = BIO_new( BIO_s_mem() );
    X509* x = NULL;
    do{
        if ( NULL == bio ) break;
        
        BIO_write( bio, data, (int)strlen( data ) );
        (void)BIO_flush( bio );
#if ( OPENSSL_VERSION_NUMBER == 0x10101040L ) // UE 4.22.3 default in windows
		x = PEM_read_bio_X509(bio, NULL, SSL_CTX_get_default_passwd_cb(SSL_get_SSL_CTX(ssl)), SSL_CTX_get_default_passwd_cb_userdata(SSL_get_SSL_CTX(ssl)));
#else
        x = PEM_read_bio_X509( bio, NULL, ssl->ctx->default_passwd_callback, ssl->ctx->default_passwd_callback_userdata );
#endif
		if ( NULL == x ) break;
        
        result = ( 1 == SSL_use_certificate( ssl, x ) );
    }while ( false );
    if ( NULL != x ) X509_free( x );
    if ( NULL != bio ) BIO_free( bio );
    return result;
}


*/
