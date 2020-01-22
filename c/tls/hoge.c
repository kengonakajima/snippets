#include <stdio.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <assert.h>

int verify_callback(int preverify_ok, X509_STORE_CTX *x509storectx ) {
    printf("verify_callback called, preverify_ok:%d x:%p\n",preverify_ok,x509storectx);
    return preverify_ok;
}

void dumpbin(const char*s, size_t l) {
    for(size_t i=0;i<l;i++){
        fprintf(stderr, "%02x ", s[i] & 0xff );
        if((i%8)==7) fprintf(stderr,"  ");
        if((i%16)==15) fprintf(stderr,"\n");
    }
    fprintf(stderr,"\n");
}


int main(int argc, char **argv ) {
    char *svcertfile = argv[1];
    char *svkeyfile = argv[2];
    char *rootca = argv[3];
    assert(svcertfile);
    assert(svkeyfile);
    assert(rootca);
    
    // server
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    
    const SSL_METHOD *svmethod = TLS_server_method(); 
    SSL_CTX *svctx = SSL_CTX_new(svmethod);
    assert(svctx);
    int ret;
    ret = SSL_CTX_use_certificate_file(svctx,svcertfile,SSL_FILETYPE_PEM);
    assert(ret>0);
    ret = SSL_CTX_use_PrivateKey_file(svctx,svkeyfile,SSL_FILETYPE_PEM);
    assert(ret>0);

    SSL *svssl = SSL_new(svctx);
    assert(svssl);
    BIO *sv_wbio = BIO_new(BIO_s_mem());
    BIO *sv_rbio = BIO_new(BIO_s_mem());

    SSL_set_bio(svssl, sv_rbio, sv_wbio);
    printf("svinit done\n");
    
    // client https://wiki.openssl.org/index.php/SSL/TLS_Client
    const SSL_METHOD *clmethod = TLS_client_method();
    SSL_CTX *clctx = SSL_CTX_new(clmethod);
    assert(clctx);
    SSL_CTX_set_verify(clctx,SSL_VERIFY_PEER,verify_callback);
    SSL_CTX_set_verify_depth(clctx, 4);
    const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
    SSL_CTX_set_options(clctx, flags);
    ret = SSL_CTX_load_verify_locations(clctx, "DST_Root_CA_X3.pem", NULL);
    assert(ret==1);

    SSL *clssl = SSL_new(clctx);
    assert(clssl);
    BIO *cl_wbio = BIO_new(BIO_s_mem());
    BIO *cl_rbio = BIO_new(BIO_s_mem());
    SSL_set_bio(clssl,cl_rbio,cl_wbio);

    const char* const PREFERRED_CIPHERS = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
    ret = SSL_set_cipher_list(clssl, PREFERRED_CIPHERS);
    assert(ret==1);

    ret = SSL_set_tlsext_host_name(clssl, "oneframe.io");
    assert(ret==1);

    //    ret = SSL_do_handshake(clssl);
    ret = SSL_connect(clssl);
    printf("ssl_connect ret:%d\n",ret);
    if(SSL_get_error(clssl,ret)==SSL_ERROR_WANT_READ) {
        printf("client want read (OK)\n");
    } else {
        printf("client must want read here\n");
        return 1;
    }
    char clhello[2048];
    int clhello_len = BIO_read(cl_wbio,clhello,sizeof(clhello));
    assert(clhello_len>0);
    printf("BIO_read(cl_wbio) clhello_len:%d\n",clhello_len);
    //    dumpbin(clhello,clhello_len);
    ret = BIO_write(sv_rbio,clhello,clhello_len);
    assert(ret==clhello_len);

    ret = SSL_accept(svssl);
    printf("ssl_accept ret:%d\n",ret);
    if(SSL_get_error(svssl,ret)==SSL_ERROR_WANT_READ) {
        printf("server want read (OK)\n");
    } else {
        printf("server must want read here\n");
        return 1;
    }

    char svhello[4096]; // hello+cert+key
    int svhello_len = BIO_read(sv_wbio,svhello,sizeof(svhello));
    assert(svhello_len>0);
    printf("BIO_read(sv_wbio) svhello_len:%d\n",svhello_len);
    //    dumpbin(svhello,svhello_len);
    ret=SSL_read(clssl,svhello,svhello_len);
    printf("ssl_read(cl) ret:%d\n",ret);
    
    ret = BIO_write(cl_rbio,svhello,svhello_len);
    assert(ret==svhello_len);
    ret = SSL_connect(clssl);
    printf("ssl_connect second call, ret:%d\n",ret);
    if(ret<0) {
        char s[1000];
        ret=SSL_get_error(clssl,ret);    
        ERR_print_errors_fp(stderr);
        return 1;
    }
    printf("ssl_connect succeeded!\n");
    printf("done\n");
    return 0;
}

