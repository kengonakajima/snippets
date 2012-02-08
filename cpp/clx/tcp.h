/* ------------------------------------------------------------------------- */
/*
 *  tcp.h
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
 *  Last-modified: Wed 22 Oct 2008 14:50:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_TCP_H
#define CLX_TCP_H

#include <string>
#include <stdexcept>
#include "resolver.h"
#include "socket.h"
#include "sockaddress.h"
#include "sockstream.h"
#include "sockmanager.h"
#include "timer.h"

namespace clx {
	namespace tcp {
		/* ----------------------------------------------------------------- */
		//  basic_socket
		/* ----------------------------------------------------------------- */
		template <int Family>
		class basic_socket : public basic_rawsocket<SOCK_STREAM, Family> {
		public:
			typedef basic_rawsocket<SOCK_STREAM, Family> rawsocket;
			typedef basic_sockaddress<Family, IPPROTO_TCP> sockaddress;
			typedef char char_type;
			typedef typename std::basic_string<char> string_type;
			
			basic_socket() : rawsocket(), addr_(), connect_(false) {}
			
			basic_socket(const basic_socket& cp) :
				rawsocket(cp), addr_(cp.addr_), connect_(cp.connect_) {}
			
			basic_socket& operator=(const basic_socket& cp) {
				addr_ = cp.addr_;
				connect_ = cp.connect_;
				this->assign(static_cast<rawsocket>(cp));
				return *this;
			}
			
			// for client socket
			explicit basic_socket(const char_type* host, int port) :
				rawsocket(), addr_(host, port), connect_(false) {
				try {
					this->xconnect();
				}
				catch (socket_error& e) {
					this->assign(rawsocket());
					throw e;
				}
			}
			
			explicit basic_socket(const string_type& host, int port) :
				rawsocket(), addr_(host, port), connect_(false) {
				try {
					this->xconnect();
				}
				catch (socket_error& e) {
					this->assign(rawsocket());
					throw e;
				}
			}
			
			// for server socket
			explicit basic_socket(socket_int s, const sockaddress& addr) :
				rawsocket(s), addr_(addr), connect_(true) {}
			
			virtual ~basic_socket() throw() {}
			
			// operator
			basic_socket& connect(const char_type* host, int port) {
				addr_.assign(host, port);
				return this->xconnect();
			}
			
			basic_socket& connect(const string_type& host, int port) {
				return this->connect(host.c_str(), port);
			}
			
			virtual void close() {
				rawsocket::close();
				connect_ = false;
			}
			
			virtual int send(const char_type* src, int n) {
				return ::send(this->socket(), src, n, 0);
			}
			
			virtual int send(const string_type& src) {
				return this->send(src.c_str(), static_cast<int>(src.size()));
			}
			
			virtual int recv(char_type* dest, int n) {
				return ::recv(this->socket(), dest, n, 0);
			}
			
			const sockaddress& address() const { return addr_; }
			bool is_connect() const { return connect_; }
		private:
			sockaddress addr_;
			bool connect_;
			
			basic_socket& xconnect() {
				if (connect_) return *this;
				if (!this->is_open()) this->open();
				if (!this->is_open()) throw clx::socket_error("socket");
				if (::connect(this->socket(), (struct sockaddr*)addr_.data(), addr_.size()) == -1) {
					throw clx::socket_error("connect");
				}
				connect_ = true;
				
				return *this;
			}
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_acceptor
		/* ----------------------------------------------------------------- */
		template <int Family>
		class basic_acceptor : public basic_rawsocket<SOCK_STREAM, Family> {
		public:
			typedef basic_rawsocket<SOCK_STREAM, Family> rawsocket;
			typedef basic_sockaddress<Family, IPPROTO_TCP> sockaddress;
			typedef char char_type;
			typedef typename std::basic_string<char> string_type;
			
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
			
			basic_socket<Family> accept() {
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
				return basic_socket<Family>(s, addr);
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
		
		/* ----------------------------------------------------------------- */
		//  port
		/* ----------------------------------------------------------------- */
		int port(const char* service) {
			return clx::port(service, "tcp");
		}
		
		int port(const std::basic_string<char>& service) {
			return clx::tcp::port(service.c_str());
		}
		
		typedef basic_rawsocket<SOCK_STREAM, AF_INET> rawsocket;
		typedef basic_socket<AF_INET> socket;
		typedef basic_acceptor<AF_INET> acceptor;
		typedef basic_sockaddress<AF_INET, IPPROTO_TCP> sockaddress;
		typedef basic_sockbuf<socket> sockbuf;
		typedef basic_sockstream<socket> sockstream;
		typedef basic_sockmanager<SOCK_STREAM, AF_INET> sockmanager;
	};
}

#endif // CLX_TCP_H
