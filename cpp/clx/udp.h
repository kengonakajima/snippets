/* ------------------------------------------------------------------------- */
/*
 *  udp.h
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
 *  Last-modified: Sat 25 Oct 2008 05:35:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UDP_H
#define CLX_UDP_H

#include <cstdlib>
#include <cstring>
#include <string>
#include "resolver.h"
#include "socket.h"
#include "sockaddress.h"
#include "sockstream.h"
#include "sockmanager.h"

namespace clx {
	namespace udp {
		/* ----------------------------------------------------------------- */
		//  basic_socket
		/* ----------------------------------------------------------------- */
		template <int Family>
		class basic_socket : public basic_rawsocket<SOCK_DGRAM, Family> {
		public:
			typedef basic_rawsocket<SOCK_DGRAM, Family> rawsocket;
			typedef basic_sockaddress<Family, IPPROTO_UDP> sockaddress;
			typedef char char_type;
			typedef std::basic_string<char> string_type;
			
			// constructor and destructor
			basic_socket() : rawsocket(), from_(), to_(), is_bind_(false) {}
			
			basic_socket(const basic_socket& cp) :
				rawsocket(cp), from_(cp.from_), to_(cp.to_), is_bind_(cp.is_bind_) {}
			
			basic_socket& operator=(const basic_socket& cp) {
				from_ = cp.from_;
				to_ = cp.to_;
				is_bind_ = cp.is_bind_;
				this->assign(cp);
			}
			
			explicit basic_socket(socket_int s, const sockaddress& addr) :
				rawsocket(s), from_(), to_(addr), is_bind_(false) {}
			
			explicit basic_socket(const char_type* host, int port) :
				rawsocket(), from_(), to_(), is_bind_(false) {
				try {
					this->connect(host, port);
				}
				catch (socket_error& e) {
					this->assign(rawsocket());
					throw e;
				}
			}
			
			explicit basic_socket(const string_type& host, int port) :
				rawsocket(), from_(), to_(), is_bind_(false) {
				try {
					this->connect(host, port);
				}
				catch (socket_error& e) {
					this->assign(rawsocket());
					throw e;
				}
			}
			
			explicit basic_socket(int port) :
				rawsocket(), from_(), to_(), is_bind_(false) {
				try {
					this->bind(port);
				}
				catch (socket_error& e) {
					this->assign(rawsocket());
					throw e;
				}
			}
			
			virtual ~basic_socket() throw() {}
			
			// operator
			basic_socket& connect(const char_type* host, int port) {
				if (this->is_open()) this->close();
				to_.assign(host, port);
				this->open();
				if (!this->is_open()) throw socket_error("open");
				return *this;
			}
			
			basic_socket& connect(const string_type& host, int port) {
				return this->connect(host.c_str(), port);
			}
			
			basic_socket& bind(int port) {
				to_.assign(port);
				return this->xbind();
			}
			
			int send_to(const char_type* src, int n, const sockaddress& addr) {
				return ::sendto(this->socket(), src, n, 0, (struct sockaddr*)addr.data(), addr.size());
			}
			
			int send_to(const string_type& src, const sockaddress& addr) {
				return this->send_to(src.c_str(), src.size(), addr);
			}
			
			int send_to(const char_type* src, int n, const char_type* host, int port) {
				sockaddress addr(host, port);
				return this->send_to(src, n, addr);
			}
			
			int send_to(const string_type& src, const string_type& host, int port) {
				sockaddress addr(host, port);
				return this->send_to(src, addr);
			}
			
			virtual int send(const char_type* src, int n) {
				int len = -1;
				
				if (!this->is_bind()) len = this->send_to(src, n, to_);
				else len = this->send_to(src, n, from_);
				return len;
			}
			
			virtual int send(const string_type& src) {
				return this->send(src.c_str(), src.size());
			}
			
			virtual int recv(char_type* dest, int n) {
				if (!this->is_bind()) return -1;
				
				typename sockaddress::inet_type tmp;
				int len = sizeof(tmp);
				std::memset(dest, 0, n);
				
				int ret = ::recvfrom(this->socket(), dest, n, 0, (struct sockaddr*)&tmp, (socklen_type*)&len);
				if (ret > 0) from_ = tmp;
				return ret;
			}
			
			// accessor
			bool is_bind() const { return is_bind_; }
			const sockaddress& from() const { return from_; }
			const sockaddress& to() const { return to_; }
			
		private:
			sockaddress from_;
			sockaddress to_;
			bool is_bind_;
			
			basic_socket& xbind() {
				if (this->is_open()) this->close();
				this->open();
				
				if (!this->is_open()) throw socket_error("open");
				if (::bind(this->socket(), (struct sockaddr*)to_.data(), to_.size()) == -1) {
					throw socket_error("bind");
				}
				
				is_bind_ = true;
				return *this;
			}
		};
		
		/* ----------------------------------------------------------------- */
		//  port
		/* ----------------------------------------------------------------- */
		int port(const char* service) {
			return clx::port(service, "udp");
		}
		
		int port(const std::basic_string<char>& service) {
			return clx::udp::port(service.c_str());
		}
		
		typedef basic_rawsocket<SOCK_DGRAM, AF_INET> rawsocket;
		typedef basic_socket<AF_INET> socket;
		typedef basic_sockaddress<AF_INET, IPPROTO_UDP> sockaddress;
		typedef basic_sockbuf<socket> sockbuf;
		typedef basic_sockstream<socket> sockstream;
		typedef basic_sockmanager<SOCK_DGRAM, AF_INET> sockmanager;
	};
}

#endif // CLX_UDP_H
