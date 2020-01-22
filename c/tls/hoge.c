#include <stdio.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <assert.h>
#include <string.h>


void print_cn_name(const char* label, X509_NAME* const name) {
    int idx = -1, success = 0;
    unsigned char *utf8 = NULL;
    do {
        if(!name) break; /* failed */
        
        idx = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
        if(!(idx > -1))  break; /* failed */
        
        X509_NAME_ENTRY* entry = X509_NAME_get_entry(name, idx);
        if(!entry) break; /* failed */
        
        ASN1_STRING* data = X509_NAME_ENTRY_get_data(entry);
        if(!data) break; /* failed */
        
        int length = ASN1_STRING_to_UTF8(&utf8, data);
        if(!utf8 || !(length > 0))  break; /* failed */
        
        fprintf(stdout, "  %s: %s\n", label, utf8);
        success = 1;
        
    } while (0);
    if(utf8) OPENSSL_free(utf8);
    if(!success) fprintf(stdout, "  %s: <not available>\n", label);
}

void print_san_name(const char* label, X509* const cert) {
    int success = 0;
    GENERAL_NAMES* names = NULL;
    unsigned char* utf8 = NULL;
    
    do {
        if(!cert) break; /* failed */
        
        names = X509_get_ext_d2i(cert, NID_subject_alt_name, 0, 0 );
        if(!names) break;
        
        int i = 0, count = sk_GENERAL_NAME_num(names);
        if(!count) break; /* failed */
        
        for( i = 0; i < count; ++i ) {
            GENERAL_NAME* entry = sk_GENERAL_NAME_value(names, i);
            if(!entry) continue;
            
            if(GEN_DNS == entry->type) {
                int len1 = 0, len2 = -1;
                
                len1 = ASN1_STRING_to_UTF8(&utf8, entry->d.dNSName);
                if(utf8) {
                    len2 = (int)strlen((const char*)utf8);
                }
                
                if(len1 != len2) {
                    fprintf(stderr, "  Strlen and ASN1_STRING size do not match (embedded null?): %d vs %d\n", len2, len1);
                }
                
                /* If there's a problem with string lengths, then     */
                /* we skip the candidate and move on to the next.     */
                /* Another policy would be to fails since it probably */
                /* indicates the client is under attack.              */
                if(utf8 && len1 && len2 && (len1 == len2)) {
                    fprintf(stdout, "  %s: %s\n", label, utf8);
                    success = 1;
                }
                
                if(utf8) {
                    OPENSSL_free(utf8), utf8 = NULL;
                }
            } else {
                fprintf(stderr, "  Unknown GENERAL_NAME type: %d\n", entry->type);
            }
        }
    } while (0);
    
    if(names) GENERAL_NAMES_free(names);
    if(utf8) OPENSSL_free(utf8);
    if(!success) fprintf(stdout, "  %s: <not available>\n", label);
}


int verify_callback(int preverify, X509_STORE_CTX* x509_ctx) {

    printf("verify_callback called, preverify_ok:%d x:%p\n",preverify,x509_ctx);    
    /* For error codes, see http://www.openssl.org/docs/apps/verify.html  */
    
    int depth = X509_STORE_CTX_get_error_depth(x509_ctx);
    int err = X509_STORE_CTX_get_error(x509_ctx);
    
    X509* cert = X509_STORE_CTX_get_current_cert(x509_ctx);
    X509_NAME* iname = cert ? X509_get_issuer_name(cert) : NULL;
    X509_NAME* sname = cert ? X509_get_subject_name(cert) : NULL;
    
    fprintf(stdout, "verify_callback (depth=%d)(preverify=%d)\n", depth, preverify);
    
    /* Issuer is the authority we trust that warrants nothing useful */
    print_cn_name("Issuer (cn)", iname);
    
    /* Subject is who the certificate is issued to by the authority  */
    print_cn_name("Subject (cn)", sname);
    
    if(depth == 0) {
        /* If depth is 0, its the server's certificate. Print the SANs */
        print_san_name("Subject (san)", cert);
    }
    
    if(preverify == 0) {
        if(err == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY)
            fprintf(stdout, "  Error = X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY\n");
        else if(err == X509_V_ERR_CERT_UNTRUSTED)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_UNTRUSTED\n");
        else if(err == X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN)
            fprintf(stdout, "  Error = X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN\n");
        else if(err == X509_V_ERR_CERT_NOT_YET_VALID)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_NOT_YET_VALID\n");
        else if(err == X509_V_ERR_CERT_HAS_EXPIRED)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_HAS_EXPIRED\n");
        else if(err == X509_V_OK)
            fprintf(stdout, "  Error = X509_V_OK\n");
        else if(err == X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE)
            fprintf(stdout, "  Error = X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE\n");
        else
            fprintf(stdout, "  Error = %d\n", err);
    }

#if !defined(NDEBUG)
    return 1;
#else
    return preverify;
#endif
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
    ret = SSL_CTX_use_certificate_chain_file(svctx,svcertfile);
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
    ret = SSL_CTX_load_verify_locations(clctx, rootca, NULL);
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
    char clhello[8192];
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

    char svhello[16384]; // hello+cert+key
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


