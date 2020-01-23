#include <stdio.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/rand.h> // RAND_bytes

#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>




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
        
        printf( "  %s: %s\n", label, utf8);
        success = 1;
        
    } while (0);
    if(utf8) OPENSSL_free(utf8);
    if(!success) printf( "  %s: <not available>\n", label);
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
                    printf( "  Strlen and ASN1_STRING size do not match (embedded null?): %d vs %d\n", len2, len1);
                }
                
                /* If there's a problem with string lengths, then     */
                /* we skip the candidate and move on to the next.     */
                /* Another policy would be to fails since it probably */
                /* indicates the client is under attack.              */
                if(utf8 && len1 && len2 && (len1 == len2)) {
                    printf( "  %s: %s\n", label, utf8);
                    success = 1;
                }
                
                if(utf8) {
                    OPENSSL_free(utf8), utf8 = NULL;
                }
            } else {
                printf( "  Unknown GENERAL_NAME type: %d\n", entry->type);
            }
        }
    } while (0);
    
    if(names) GENERAL_NAMES_free(names);
    if(utf8) OPENSSL_free(utf8);
    if(!success) printf( "  %s: <not available>\n", label);
}


int verify_callback(int preverify, X509_STORE_CTX* x509_ctx) {

    printf("verify_callback called, preverify_ok:%d x:%p\n",preverify,x509_ctx);    
    /* For error codes, see http://www.openssl.org/docs/apps/verify.html  */
    
    int depth = X509_STORE_CTX_get_error_depth(x509_ctx);
    int err = X509_STORE_CTX_get_error(x509_ctx);
    
    X509* cert = X509_STORE_CTX_get_current_cert(x509_ctx);
    X509_NAME* iname = cert ? X509_get_issuer_name(cert) : NULL;
    X509_NAME* sname = cert ? X509_get_subject_name(cert) : NULL;
    
    printf( "verify_callback (depth=%d)(preverify=%d)\n", depth, preverify);
    
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
            printf( "  Error = X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY\n");
        else if(err == X509_V_ERR_CERT_UNTRUSTED)
            printf( "  Error = X509_V_ERR_CERT_UNTRUSTED\n");
        else if(err == X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN)
            printf( "  Error = X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN\n");
        else if(err == X509_V_ERR_CERT_NOT_YET_VALID)
            printf( "  Error = X509_V_ERR_CERT_NOT_YET_VALID\n");
        else if(err == X509_V_ERR_CERT_HAS_EXPIRED)
            printf( "  Error = X509_V_ERR_CERT_HAS_EXPIRED\n");
        else if(err == X509_V_OK)
            printf( "  Error = X509_V_OK\n");
        else if(err == X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE)
            printf( "  Error = X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE\n");
        else
            printf( "  Error = %d\n", err);
    }

#if !defined(NDEBUG)
    return 1;
#else
    return preverify;
#endif
}




void dumpbin(const char*s, size_t l) {
    for(size_t i=0;i<l;i++){
        printf( "%02x ", s[i] & 0xff );
        if((i%8)==7) printf("  ");
        if((i%16)==15) printf("\n");
    }
    printf("\n");
}


int test_tls(char*svcertfile,char*svkeyfile,char*rootca) {
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

    //    ret = SSL_set_tlsext_host_name(clssl, "oneframe.io");
    //    assert(ret==1);

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

    
    ret = BIO_write(cl_rbio,svhello,svhello_len);
    assert(ret==svhello_len);
    ret = SSL_connect(clssl);
    printf("ssl_connect second call, ret:%d\n",ret);
    if(ret<0) {
        ret=SSL_get_error(clssl,ret);    
        ERR_print_errors_fp(stderr);
        return 1;
    }
    printf("ssl_connect succeeded!\n");

    // read/write test
    for(int i=0;i<10;i++) {
        ret=SSL_write(clssl,"hellohellohello",5*3);
        printf("ssl_write(cl) ret:%d\n",ret);
        char helloenc[1024];
        ret=BIO_read(cl_wbio,helloenc,sizeof(helloenc));
        printf("bio_read(cl_wbio) ret:%d\n",ret);
    }    
    printf("TLS test done\n");

    // DTLS test
    
    return 0;
}

int cookie_initialized=0;
#define COOKIE_SECRET_LENGTH 16
unsigned char cookie_secret[COOKIE_SECRET_LENGTH];

int generate_cookie(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len) {
	unsigned char *buffer, result[EVP_MAX_MD_SIZE];
	unsigned int length = 0, resultlength;
	union {
		struct sockaddr_storage ss;
		struct sockaddr_in6 s6;
		struct sockaddr_in s4;
	} peer;

	/* Initialize a random secret */
	if (!cookie_initialized) {
		if (!RAND_bytes(cookie_secret, COOKIE_SECRET_LENGTH)) {
			printf("error setting random cookie secret\n");
			return 0;
        }
		cookie_initialized = 1;
    }

	/* Read peer information */
	(void) BIO_dgram_get_peer(SSL_get_rbio(ssl), &peer);

	/* Create buffer with peer's address and port */
	length = 0;
	switch (peer.ss.ss_family) {
		case AF_INET:
			length += sizeof(struct in_addr);
			break;
		case AF_INET6:
			length += sizeof(struct in6_addr);
			break;
		default:
            fprintf(stderr, "generate_cookie: invalid ss_family\n");
			OPENSSL_assert(0);
			break;
	}
	length += sizeof(in_port_t);
	buffer = (unsigned char*) OPENSSL_malloc(length);

	if (buffer == NULL) {
		printf("out of memory\n");
		return 0;
    }

	switch (peer.ss.ss_family) {
		case AF_INET:
			memcpy(buffer,
				   &peer.s4.sin_port,
				   sizeof(in_port_t));
			memcpy(buffer + sizeof(peer.s4.sin_port),
				   &peer.s4.sin_addr,
				   sizeof(struct in_addr));
			break;
		case AF_INET6:
			memcpy(buffer,
				   &peer.s6.sin6_port,
				   sizeof(in_port_t));
			memcpy(buffer + sizeof(in_port_t),
				   &peer.s6.sin6_addr,
				   sizeof(struct in6_addr));
			break;
		default:
			OPENSSL_assert(0);
			break;
	}

	/* Calculate HMAC of buffer using the secret */
	HMAC(EVP_sha1(), (const void*) cookie_secret, COOKIE_SECRET_LENGTH,
		 (const unsigned char*) buffer, length, result, &resultlength);
	OPENSSL_free(buffer);

	memcpy(cookie, result, resultlength);
	*cookie_len = resultlength;

	return 1;
}


int verify_cookie(SSL *ssl, const unsigned char *cookie, unsigned int cookie_len) {
	unsigned char *buffer, result[EVP_MAX_MD_SIZE];
	unsigned int length = 0, resultlength;
	union {
		struct sockaddr_storage ss;
		struct sockaddr_in6 s6;
		struct sockaddr_in s4;
	} peer;

	/* If secret isn't initialized yet, the cookie can't be valid */
	if (!cookie_initialized)
		return 0;

	/* Read peer information */
	(void) BIO_dgram_get_peer(SSL_get_rbio(ssl), &peer);

	/* Create buffer with peer's address and port */
	length = 0;
	switch (peer.ss.ss_family) {
    case AF_INET:
        length += sizeof(struct in_addr);
        break;
    case AF_INET6:
        length += sizeof(struct in6_addr);
        break;
    default:
        OPENSSL_assert(0);
        break;
	}
	length += sizeof(in_port_t);
	buffer = (unsigned char*) OPENSSL_malloc(length);

	if (buffer == NULL) {
        printf("out of memory\n");
        return 0;
    }

	switch (peer.ss.ss_family) {
    case AF_INET:
        memcpy(buffer,
               &peer.s4.sin_port,
               sizeof(in_port_t));
        memcpy(buffer + sizeof(in_port_t),
               &peer.s4.sin_addr,
               sizeof(struct in_addr));
        break;
    case AF_INET6:
        memcpy(buffer,
               &peer.s6.sin6_port,
               sizeof(in_port_t));
        memcpy(buffer + sizeof(in_port_t),
               &peer.s6.sin6_addr,
               sizeof(struct in6_addr));
        break;
    default:
        OPENSSL_assert(0);
        break;
	}

	/* Calculate HMAC of buffer using the secret */
	HMAC(EVP_sha1(), (const void*) cookie_secret, COOKIE_SECRET_LENGTH,
		 (const unsigned char*) buffer, length, result, &resultlength);
	OPENSSL_free(buffer);

	if (cookie_len == resultlength && memcmp(result, cookie, resultlength) == 0) return 1;

	return 0;
}

/////////////////////////


typedef struct custom_bio_data_st {
    BIO *membio;
    int peekmode;
} custom_bio_data_t;


long BIO_s_hoge_ctrl(BIO *b, int cmd, long larg, void *pargs) {
    custom_bio_data_t *ptr = (custom_bio_data_t*)BIO_get_data(b);
    assert(ptr);
    long ret = 0;
    printf( "BIO_s_hoge_ctrl(BIO[%p], cmd[%d], larg[%ld], pargs[%p])\n", b, cmd, larg, pargs);

    switch(cmd)
    {
        case BIO_CTRL_FLUSH: // 11
            ret=BIO_ctrl(ptr->membio,BIO_CTRL_FLUSH,0,0);
            break;
        case BIO_CTRL_DGRAM_SET_CONNECTED: // 32
        case BIO_CTRL_DGRAM_SET_PEER: // 44
            ret = 1;
        case BIO_CTRL_DGRAM_GET_PEER: // 46
            printf( "BIO_CTRL_DGRAM_GET_PEER called\n");
            {
                struct sockaddr_in dummy_saddr;
                ret=sizeof(dummy_saddr);
                memset(&dummy_saddr,0,sizeof(dummy_saddr));
                dummy_saddr.sin_family=AF_INET;
                memcpy(pargs,&dummy_saddr,sizeof(dummy_saddr));
            }
            break;
        case BIO_CTRL_WPENDING: // 13
            ret = 0;
            break;
        case BIO_CTRL_DGRAM_QUERY_MTU: // 40
        case BIO_CTRL_DGRAM_GET_FALLBACK_MTU: // 47
            ret = 1400;
            break;
        case BIO_CTRL_DGRAM_GET_MTU_OVERHEAD: // 49
            ret = 48; // random guess
            // bss_dgram.cでは af_inetだと28　inet6だと48 デフォルトで28　になっている
            break;
        case BIO_CTRL_DGRAM_SET_PEEK_MODE: // 71
            ptr->peekmode = !!larg;
            ret = 1;
            break;
        case BIO_CTRL_PUSH: // 6
        case BIO_CTRL_POP: // 7
        case BIO_CTRL_DGRAM_SET_NEXT_TIMEOUT: // 45
            ret = 0;
            break;
        default:
            printf( "BIO_s_hoge_ctrl(BIO[%p], cmd[%d], larg[%ld], pargs[%p])\n", b, cmd, larg, pargs);
            printf( "  unknown cmd: %d\n", cmd);
            ret = 0;
            assert(0);
            //            raise(SIGTRAP);
            break;
    }
    printf("BIO_s_hoge_ctrl ret:%d\n",(int)ret);
    return ret;
}
int BIO_s_hoge_create(BIO *b) {
    printf( "BIO_s_hoge_create(BIO[%p])\n", b);
    custom_bio_data_t *data= (custom_bio_data_t*)malloc(sizeof(custom_bio_data_t));
    data->peekmode=0;
    data->membio= BIO_new(BIO_s_mem());
    BIO_set_data(b,data);
    BIO_set_init(b, 1);
    return 1;
}
int BIO_s_hoge_destroy(BIO *b) {
    printf( "BIO_s_hoge_destroy(BIO[%p])\n", b);
    custom_bio_data_t *data=BIO_get_data(b);
    BIO_free(data->membio);
    return 1;
}
int BIO_s_hoge_write(BIO *b, const char *data, int dlen) {
    printf( "BIO_s_hoge_write(BIO[%p], data[%p], dlen[%d])\n", b, data, dlen);        
    custom_bio_data_t *d = (custom_bio_data_t*)BIO_get_data(b);
    int ret=BIO_write(d->membio,data,dlen);
    printf("BIO_s_hoge_write ret: %d\n",ret);
    return ret;
}


int BIO_s_hoge_read(BIO *b, char *data, int dlen) {
    printf( "BIO_s_hoge_read(BIO[%p], data[%p], dlen[%d])\n", b, data, dlen);    
    custom_bio_data_t *ptr= (custom_bio_data_t*)BIO_get_data(b);    
    assert(ptr);
    int ret = BIO_read(ptr->membio,data,dlen);
    printf("bio_s_hoge_read: bio_read ret:%d\n",ret);
    if(ret<=0) {
        printf("setting retry read\n");
        BIO_set_retry_read(b);         // need this
        return 0;
    }
    if(ret<0) ERR_print_errors_fp(stderr);
    if(ret<0) ERR_print_errors(ptr->membio);
    if(ret<0) ERR_print_errors(b);
    return ret;
}

BIO_METHOD *bio_hoge=NULL;

const BIO_METHOD *BIO_s_hoge() {
    if(bio_hoge) return bio_hoge;
    bio_hoge = BIO_meth_new(BIO_get_new_index()| BIO_TYPE_SOURCE_SINK,"BIO_s_hoge");
    assert(bio_hoge);
    BIO_meth_set_write(bio_hoge,BIO_s_hoge_write);
    BIO_meth_set_read(bio_hoge,BIO_s_hoge_read);    
    BIO_meth_set_ctrl(bio_hoge,BIO_s_hoge_ctrl);
    BIO_meth_set_create(bio_hoge,BIO_s_hoge_create);
    BIO_meth_set_destroy(bio_hoge,BIO_s_hoge_destroy);        
    
                            
    return bio_hoge; //BIO_s_mem();
}
int test_dtls(char*svcertfile,char*svkeyfile,char*rootca) {
    const SSL_METHOD *svmethod = DTLS_server_method(); 
    SSL_CTX *svctx = SSL_CTX_new(svmethod);
    assert(svctx);
    int ret;
    ret = SSL_CTX_use_certificate_chain_file(svctx,svcertfile);
    assert(ret>0);
    ret = SSL_CTX_use_PrivateKey_file(svctx,svkeyfile,SSL_FILETYPE_PEM);
    assert(ret>0);

	SSL_CTX_set_session_cache_mode(svctx, SSL_SESS_CACHE_OFF);
    ret = SSL_CTX_check_private_key(svctx);
    assert(ret);

    // omit client auth
	// SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, dtls_verify_callback);
    
	SSL_CTX_set_read_ahead(svctx, 1);
	SSL_CTX_set_cookie_generate_cb(svctx, generate_cookie);
	SSL_CTX_set_cookie_verify_cb(svctx, &verify_cookie);
    
    union {
		struct sockaddr_storage ss;
		struct sockaddr_in s4;
		struct sockaddr_in6 s6;
	} sv_remote_addr, sv_local_addr, cl_remote_addr, cl_local_addr;    

    SSL *svssl = SSL_new(svctx);
    assert(svssl);
    printf("svssl:%p\n",svssl);
    BIO *sv_wbio = BIO_new(BIO_s_hoge());
    BIO *sv_rbio = BIO_new(BIO_s_hoge());

    SSL_set_bio(svssl, sv_rbio, sv_wbio);
	BIO_ctrl(sv_rbio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &sv_remote_addr.ss);
	BIO_ctrl(sv_wbio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &sv_remote_addr.ss);    

    SSL_set_options(svssl, SSL_OP_COOKIE_EXCHANGE);

    printf("d svinit done\n");

    // client https://wiki.openssl.org/index.php/SSL/TLS_Client
    const SSL_METHOD *clmethod = DTLS_client_method();
    SSL_CTX *clctx = SSL_CTX_new(clmethod);
    assert(clctx);
    SSL_CTX_set_verify(clctx,SSL_VERIFY_PEER,verify_callback);
    SSL_CTX_set_verify_depth(clctx, 4);
    const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
    SSL_CTX_set_options(clctx, flags);
    ret = SSL_CTX_load_verify_locations(clctx, rootca, NULL);
    assert(ret==1);
	SSL_CTX_set_read_ahead(clctx, 1);

    SSL *clssl = SSL_new(clctx);
    assert(clssl);
    printf("clssl:%p\n",clssl);
    BIO *cl_wbio = BIO_new(BIO_s_hoge());
    BIO *cl_rbio = BIO_new(BIO_s_hoge());
    SSL_set_bio(clssl,cl_rbio,cl_wbio);
	BIO_ctrl(cl_rbio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &cl_remote_addr.ss);
	BIO_ctrl(cl_wbio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &cl_remote_addr.ss);
    
    const char* const PREFERRED_CIPHERS = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
    ret = SSL_set_cipher_list(clssl, PREFERRED_CIPHERS);
    assert(ret==1);

    //    ret = SSL_set_tlsext_host_name(clssl, "oneframe.io");
    //    assert(ret==1);

    //    printf("calling ssl_connect\n");
    //    ret = SSL_connect(clssl);
    //    printf("ssl_connect ret:%d\n",ret);
    SSL_set_connect_state(clssl);

    ret = SSL_do_handshake(clssl);
    printf("ssl_do_handshake: ret:%d\n",ret);
    
    int e;
    if((e=SSL_get_error(clssl,ret))==SSL_ERROR_WANT_READ) {
        printf("client want read (OK)\n");
    } else {
        if(e==SSL_ERROR_SYSCALL) {
            printf("SSL_ERROR_SYSCALL errno:%d\n",errno);
        }
        printf("client must want read ecode:%d \n",e);
        ERR_print_errors_fp(stderr);
        return 1;
    }
    custom_bio_data_t *cl_wptr = BIO_get_data(cl_wbio);
    char clhello[2048];
    int clhello_len = BIO_read(cl_wptr->membio,clhello,sizeof(clhello));
    printf("BIO_read(cl_wptr->membio) clhello_len:%d\n",clhello_len);

    custom_bio_data_t *sv_rptr = BIO_get_data(sv_rbio);
    ret = BIO_write(sv_rptr->membio,clhello,clhello_len);
    assert(ret==clhello_len);

    ret = SSL_accept(svssl);
    printf("ssl_accept ret:%d\n", ret);
    if(SSL_get_error(svssl,ret)==SSL_ERROR_WANT_READ) {
        printf("server want read (OK)\n");
    } else {
        printf("server must want read here\n");
        return 1;
    }

    custom_bio_data_t *sv_wptr = BIO_get_data(sv_wbio);
    char svhello[2048];
    int svhello_len = BIO_read(sv_wptr->membio,svhello,sizeof(svhello));
    assert(svhello_len>0);
    printf("BIO_read(sv_wptr->membio) svhello_len:%d\n",svhello_len);
    dumpbin(svhello,svhello_len);

    custom_bio_data_t *cl_rptr = BIO_get_data(cl_rbio);
    ret = BIO_write(cl_rptr->membio,svhello,svhello_len);
    assert(ret==svhello_len);
    ret = SSL_connect(clssl);
    printf("ssl_connect second call, ret:%d\n",ret);
    if(SSL_get_error(clssl,ret)==SSL_ERROR_WANT_READ) {
        printf("client want read (OK)\n");
    } else {
        printf("client must want read\n");
        ret=SSL_get_error(clssl,ret);    
        ERR_print_errors_fp(stderr);
        return 1;        
    }

    char cl_hello2[2048]; 
    int cl_hello2_len = BIO_read(cl_wptr->membio,cl_hello2,sizeof(cl_hello2));
    assert(cl_hello2_len>0);
    ret=BIO_write(sv_rptr->membio,cl_hello2,cl_hello2_len);
    assert(ret==cl_hello2_len);

    ret = SSL_accept(svssl);
    printf("ssl_accept second ret:%d\n", ret);
    if(SSL_get_error(svssl,ret)==SSL_ERROR_WANT_READ) {
        printf("server want read (OK)\n");        
    } else {
        printf("server must want read\n");
        return 1;
    }

    char sv_hello2[2048];
    int sv_hello2_len = BIO_read(sv_wptr->membio,sv_hello2,sizeof(sv_hello2));
    assert(sv_hello2_len>0);
    ret=BIO_write(cl_rptr->membio,sv_hello2,sv_hello2_len);
    assert(ret==sv_hello2_len);

    ret = SSL_connect(clssl);
    printf("ssl_connect third ret:%d\n",ret);
    if(SSL_get_error(clssl,ret)==SSL_ERROR_WANT_READ) {
        printf("client want read (OK)\n");
    } else {
        printf("client must want read\n");
        return 1;
    }

    char cl_hello3[2048];
    int cl_hello3_len = BIO_read(cl_wptr->membio,cl_hello3,sizeof(cl_hello3));
    assert(cl_hello3_len>0);
    ret=BIO_write(sv_rptr->membio,cl_hello3,cl_hello3_len);
    assert(ret==cl_hello3_len);

    ret = SSL_accept(svssl);
    printf("SSL_accept fourth ret:%d\n",ret);
    if(ret>0) {
        printf("SSL_connect DONE!\n");
    } else {
        printf("SSL_connect failed!\n");
    }

    const char *cl_protov = SSL_get_version(clssl);
    const char *sv_protov = SSL_get_version(svssl);
    printf("VER: cl:%s sv:%s\n",cl_protov,sv_protov);

    
    // read write test
    int cnt=0;
    while(1) {
        cnt++;
        if(cnt>10)break;
                      
        printf("calling ssl_write from client %p\n",clssl);
        ret=SSL_write(clssl,"hellohellohello",5*3);
        int e;
        if((e=SSL_get_error(clssl,ret))==SSL_ERROR_WANT_READ) {
            printf("client want read\n");
        } else {
            printf("client error: %d\n",e);
            ERR_print_errors_fp(stderr);
        }

        char svreadbuf[2048];
        ret=SSL_read(svssl,svreadbuf,sizeof(svreadbuf));
        if(ret>0) {
            svreadbuf[ret]='\0';
            printf("server read len:%d data:%s\n",ret, svreadbuf);
        }

        // manual polling
        char buf[2048];
        int svwsz=BIO_read(sv_wptr->membio,buf,sizeof(buf));
        if(svwsz>0) {
            printf("server wrote something: %d\n",svwsz);
            ret=BIO_write(cl_rptr->membio,buf,svwsz);
            assert(ret==svwsz);
        }
        int clwsz=BIO_read(cl_wptr->membio,buf,sizeof(buf));
        if(clwsz>0) {
            printf("client wrote something: %d\n",clwsz);
            ret=BIO_write(sv_rptr->membio,buf,clwsz);
            assert(ret==clwsz);
        }

        sleep(1);        
    }

    printf("DTLS test done\n");
    
    return 0;
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

    //   test_tls(svcertfile,svkeyfile,rootca);
    test_dtls(svcertfile,svkeyfile,rootca);
}
