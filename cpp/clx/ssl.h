/* ------------------------------------------------------------------------- */
/*
 *  ssl.h
 *
 *  Copyright (c) 2004 - 2008, clown. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - No names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  The source code The source code is derived from OpenSSL project.
 *  (See the following URL: http://www.openssl.org/source/license.html)
 *
 *  Last-modified: Sat 25 Oct 2008 04:50:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SSL_H
#define CLX_SSL_H

#include <ctime>
#include <string>
#include <stdexcept>
#include <new>
#include "socket.h"
#include "sockaddress.h"
#include "sockstream.h"
#include "random.h"
#include "timer.h"
#include "exception.h"

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#ifdef CLX_WIN32
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif // CLX_WIN32

namespace clx {
	namespace detail {
		class ssl_init {
		public:
			ssl_init() {
				SSL_load_error_strings();
				SSL_library_init();
			}
			
			~ssl_init() {
				ERR_free_strings();
			}
			
		private:
			ssl_init(const ssl_init& cp);
			ssl_init& operator=(const ssl_init& cp);
		};
		
		static ssl_init ssl_init_;
	}
	
	/* --------------------------------------------------------------------- */
	//  verify_error
	/* --------------------------------------------------------------------- */
	class verify_error : public std::runtime_error {
	public:
		typedef char char_type;
		typedef std::string string_type;
		typedef std::stringstream sstream_type;
		
		static const int nbuf = 1024;
		
		explicit verify_error(int status) :
			std::runtime_error("certification failed"), errno_(status) {}
		
		virtual ~verify_error() throw() {}
		
		virtual const char_type* what() {
			sstream_type tmp;
			tmp << std::runtime_error::what() << ": status 0x" << std::hex << errno_;
			return tmp.str().c_str();
		}
		
		int code() const { return errno_; }
		
	private:
		int errno_;
	};
	
	namespace ssl {
		/* ----------------------------------------------------------------- */
		//  TLS/SSL version
		/* ----------------------------------------------------------------- */
		enum { sslv2 = 0, sslv3, tlsv1, sslv23 };
		
		/* ----------------------------------------------------------------- */
		//  basic_socket
		/* ----------------------------------------------------------------- */
		template <
			int Version,
			int Family = AF_INET
		>
		class basic_socket : public basic_rawsocket<SOCK_STREAM, Family, 0> {
		public:
			typedef basic_rawsocket<SOCK_STREAM, Family, 0> rawsocket;
			typedef SSL handler;
			typedef basic_sockaddress<Family, IPPROTO_TCP> sockaddress;
			typedef char char_type;
			typedef typename std::basic_string<char> string_type;
			
			// constructor and destructor
			basic_socket() : rawsocket(), p_(NULL), addr_() {}
			
			basic_socket(const basic_socket& cp) : rawsocket(cp), p_(NULL), addr_(cp.addr_) {
				p_ = cp.p_;
				if (cp.p_ != NULL) p_->increment();
			}
			
			basic_socket& operator=(const basic_socket& cp) {
				this->decrement();
				addr_ = cp.addr_;
				this->assign(static_cast<rawsocket>(cp));
				p_ = cp.p_;
				if (cp.p_ != NULL) p_->increment();
				return *this;
			}
			
			// for client ssl socket
			explicit basic_socket(const char_type* host, int port) :
				rawsocket(), p_(NULL), addr_(host, port) {
				try {
					this->ssl_init(client);
					this->xconnect();
				}
				catch (socket_error& e) {
					(*this) = basic_socket();
					throw e;
				}
				catch (std::runtime_error& e) {
					(*this) = basic_socket();
					throw e;
				}
			}
			
			explicit basic_socket(const string_type& host, int port) :
				rawsocket(), p_(NULL), addr_(host, port) {
				try {
					this->ssl_init(client);
					this->xconnect();
				}
				catch (socket_error& e) {
					(*this) = basic_socket();
					throw e;
				}
				catch (std::runtime_error& e) {
					(*this) = basic_socket();
					throw e;
				}
			}
			
			// for server ssl socket
			explicit basic_socket(socket_int s, const sockaddress& addr) :
				rawsocket(s), p_(NULL), addr_(addr) {
				try {
					this->ssl_init(client);
					p_->connect() = true;
				}
				catch (socket_error& e) {
					(*this) = basic_socket();
					throw e;
				}
				catch (std::runtime_error& e) {
					(*this) = basic_socket();
					throw e;
				}
			}
			
			virtual ~basic_socket() throw() { this->decrement();}
			
			// for client ssl socket
			basic_socket& connect(const char_type* host, int port) {
				addr_.assign(host, port);
				return this->xconnect();
			}
			
			basic_socket& connect(const string_type& host, int port) {
				return this->connect(host.c_str(), port);
			}
			
			virtual int send(const char_type* src, int n) {
				if (p_ == NULL || p_->ssl() == NULL) return -1;
				return ::SSL_write(p_->ssl(), src, n);
			}
			
			virtual int send(const string_type& src) {
				return this->send(src.c_str(), static_cast<int>(src.size()));
			}
			
			virtual int recv(char_type* dest, int n) {
				if (p_ == NULL || p_->ssl() == NULL) return -1;
				return ::SSL_read(p_->ssl(), dest, n);
			}
			
			virtual void close() {
				rawsocket::close();
				if (p_ != NULL) {
					this->ssl_finish();
					p_->verify() = false;
					p_->connect() = false;
				}
			}
			
			bool verify_locations(const char_type* file, const char_type* dir = NULL) {
				if (p_ == NULL || p_->ctx() == NULL) this->ssl_init(client);
				if (SSL_CTX_load_verify_locations(p_->ctx(), file, dir) == 0) return false;
				p_->verify() = true;
				return true;
			}
			
			bool verify_locations(const string_type& file, const string_type& dir = string_type()) {
				if (file.empty() && dir.empty()) return false;
				else if (dir.empty()) return this->verify_locations(file.c_str(), NULL);
				else if (file.empty()) return this->verify_locations(NULL, dir.c_str());
				else return this->verify_locations(file.c_str(), dir.c_str());
			}
			
			bool is_connect() const { return (p_ != NULL) ? p_->connect() : false; }
			const sockaddress& address() const { return addr_; }
			handler* ssl() { return (p_ != NULL) ? p_->ssl() : NULL; }
			const handler* ssl() const { return (p_ != NULL) ? p_->ssl() : NULL; }
			
		private:
			typedef SSL_CTX ctx_type;
			
			enum { client = 0, server = 1};
			
			class shared_counter {
			public:
				typedef long count_type;
				typedef ctx_type* ctx_pointer;
				typedef handler* handle_pointer;
				
				shared_counter() : ctx_(NULL), ssl_(NULL), connect_(false), verify_(false), count_(1) {}
				virtual ~shared_counter() {}
				
				ctx_pointer& ctx() { return ctx_; }
				handle_pointer& ssl() { return ssl_; }
				bool& connect() { return connect_; }
				bool& verify() { return verify_; }
				
				count_type count() const { return count_; }
				count_type increment() { return ++count_; }
				count_type decrement() { return --count_; }
				
			private:
				ctx_pointer ctx_;
				handle_pointer ssl_;
				bool connect_;
				bool verify_;
				count_type count_;
			};
			
			shared_counter* p_;
			sockaddress addr_;
			
			void ssl_init(int mode) {
				SSL_METHOD *(*f)() = NULL;
				switch (Version) {
				case sslv2:
					if (mode == client) f = SSLv2_client_method;
					else f = SSLv2_server_method;
					break;
				case sslv3:
					if (mode == client) f = SSLv3_client_method;
					else f = SSLv3_server_method;
					break;
				case tlsv1:
					if (mode == client) f = TLSv1_client_method;
					else f = TLSv1_server_method;
					break;
				case sslv23:
				default:
					if (mode == client) f = SSLv23_client_method;
					else f = SSLv23_server_method;
					break;
				}
				
				if (p_ == NULL) p_ = new shared_counter();
				if (p_ == NULL) throw std::bad_alloc();
				
				if (p_->ctx() == NULL) p_->ctx() = SSL_CTX_new(f());
				if (p_->ctx() == NULL) throw std::runtime_error("SSL_CTX_new failed");
				
				if (!this->is_open()) this->open();
				if (!this->is_open()) throw socket_error("open");
				
				char err[1024];
				if (p_->ssl() == NULL) p_->ssl() = SSL_new(p_->ctx());
				if (p_->ssl() == NULL || SSL_set_fd(p_->ssl(), this->socket()) == 0) {
					std::memset(err, '\0', sizeof(err));
					ERR_error_string(ERR_get_error(), err);
					throw std::runtime_error(err);
				}
			}
			
			void ssl_finish() {
				if (p_ != NULL) {
					if (p_->ssl() != NULL) {
						SSL_shutdown(p_->ssl());
						SSL_free(p_->ssl());
						p_->ssl() = NULL;
					}
					if (p_->ctx() != NULL) {
						SSL_CTX_free(p_->ctx());
						p_->ctx() = NULL;
					}
				}
			}
			
			// for client ssl socket
			basic_socket& xconnect() {
				if (p_ == NULL || p_->ctx() == NULL || p_->ssl() == NULL) this->ssl_init(client);
				else if (p_->connect()) return *this;
				
				if (::connect(this->socket(), (struct sockaddr*)addr_.data(), addr_.size()) == -1) {
					throw clx::socket_error("connect");
				}
				p_->connect() = true;
				
				RAND_poll();
				mt19937 engine(static_cast<unsigned long>(std::time(NULL)));
				random<unsigned short> rng(engine);
				while (RAND_status() == 0) {
					unsigned short value = rng() % 65536;
					RAND_seed(&value, sizeof(value));
				}
				
				char err[1024];
				if (SSL_connect(p_->ssl()) <= 0) {
					std::memset(err, '\0', sizeof(err));
					ERR_error_string(ERR_get_error(), err);
					throw std::runtime_error(err);
				}
				
				if (p_->verify()) {
					int status = SSL_get_verify_result(p_->ssl());
					if (status != X509_V_OK) throw verify_error(status);
				}
				
				return *this;
			}
			
			void decrement() {
				if (p_ != NULL && p_->decrement() == 0) {
					this->ssl_finish();
					delete p_;
					p_ = NULL;
				}
			}
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_acceptor
		/* ----------------------------------------------------------------- */
		template <
			int Version,
			int Family = AF_INET
		>
		class basic_acceptor : public basic_rawsocket<SOCK_STREAM, Family> {
		public:
			typedef basic_rawsocket<SOCK_STREAM, Family> rawsocket;
			typedef basic_sockaddress<Family, IPPROTO_TCP> sockaddress;
			typedef char char_type;
			typedef typename std::basic_string<char> string_type;
			
			enum { pem = SSL_FILETYPE_PEM, asn1 = SSL_FILETYPE_ASN1 };
			enum { nmax = 100 };
			
			// constructor and destructor
			basic_acceptor() : rawsocket(), nlisten_(nmax) {}
			
			explicit basic_acceptor(int port, int n = nmax) :
				rawsocket(), addr_(port), nlisten_(nmax) {
				try {
					this->xopen();
				}
				catch (socket_error& e) {
					this->assign(rawsocket());
					throw e;
				}
			}
			
			virtual ~basic_acceptor() throw() {}
			
			// operator
			basic_acceptor& open(int port, int n = nmax) {
				addr_.assign(port);
				nlisten_ = n;
				return this->xopen();
			}
			
			basic_socket<Version, Family> accept(const char_type* cert, const char_type* priv,
				int type = pem) {
				socket_int s = -1;
				sockaddress addr;
				typename sockaddress::inet_type tmp;
				socklen_type len = sizeof(tmp);
				
				for (int foo = 0; foo < 10; ++foo) {
					s = ::accept(this->socket(), (struct sockaddr*)&tmp, &len);
					if (s == -1) clx::sleep(1.0);
					else break;
				}
				addr = tmp;
				
				basic_socket<Version, Family> client(s, addr);
				SSL_use_certificate_file(client.ssl(), cert, type);
				SSL_use_PrivateKey_file(client.ssl(), priv, type);
				
				char err[1024];
				if (SSL_accept(client.ssl()) <= 0) {
					std::memset(err, '\0', sizeof(err));
					ERR_error_string(ERR_get_error(), err);
					throw std::runtime_error(err);
				}
				
				return client;
			}
			
			basic_socket<Version, Family> accept(const string_type& cert, const string_type& priv,
				int type = pem) {
				return this->accept(cert.c_str(), priv.c_str(), type);
			}
			
		private:
			sockaddress addr_;
			int nlisten_;
			
			basic_acceptor& xopen() {
				if (rawsocket::open() < 0) throw socket_error("socket");
				if (::bind(this->socket(), (struct sockaddr*)addr_.data(), addr_.size()) == -1) {
					throw socket_error("bind");
				}
				if (::listen(this->socket(), nlisten_) == -1) throw socket_error("listen");
				return *this;
			}
		};
		
		typedef basic_socket<sslv23, AF_INET> socket;
		typedef basic_acceptor<sslv23, AF_INET> acceptor;
		typedef basic_sockbuf<socket> sockbuf;
		typedef basic_sockstream<socket> sockstream;
	}
}

#endif // CLX_SSL_H
