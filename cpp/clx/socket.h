/* ------------------------------------------------------------------------- */
/*
 *  socket.h
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
 *  Last-modified: Sat 25 Oct 2008 04:28:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SOCKET_H
#define CLX_SOCKET_H

#ifndef CLX_WIN32
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define CLX_WIN32
#endif
#endif // CLX_WIN32

#ifdef CLX_WIN32
#include "win32/socket.h"
#else
#include "unix/socket.h"
#endif // CLX_WIN32

#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include "exception.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_rawsocket
	/* --------------------------------------------------------------------- */
	template <
		int Type,
		int Family,
		int Protocol = 0
	>
	class basic_rawsocket {
	public:
		// constructor and destructor
		basic_rawsocket() : p_(NULL) {}
		
		basic_rawsocket(const basic_rawsocket& cp) : p_(NULL) {
			this->assign(cp);
		}
		
		explicit basic_rawsocket(socket_int s) : p_(NULL) {
			this->assign(s);
		}
		
		virtual ~basic_rawsocket() throw() { this->decrement(); }
		
		basic_rawsocket& operator=(const basic_rawsocket& cp) {
			return this->assign(cp);
		}
		
		basic_rawsocket& operator=(socket_int s) {
			return this->assign(s);
		}
		
		// operator
		basic_rawsocket& assign(const basic_rawsocket& cp) {
			this->decrement();
			p_ = cp.p_;
			if (cp.p_ != NULL) p_->increment();
			return *this;
		}
		
		basic_rawsocket& assign(socket_int s) {
			this->decrement();
			if (s >= 0) p_ = new shared_counter(s);
			if (p_ == NULL) throw std::bad_alloc();
			return *this;
		}
		
		virtual socket_int open() {
			this->decrement();
			socket_int s = open_socket(Family, Type, Protocol);
			
			p_ = new shared_counter(s);
			if (p_ == NULL) throw std::bad_alloc();
			return p_->socket();
		}
		
		virtual void close() {
			if (p_ != NULL && p_->socket() >= 0) {
				close_socket(p_->socket());
				p_->socket() = -1;
			}
		}
		
		bool is_open() const { return (p_ != NULL) ? p_->socket() >= 0 : false; }
		socket_int socket() const { return (p_ != NULL) ? p_->socket() : -1; }
		
	private:
		class shared_counter {
		public:
			typedef long count_type;
			
			shared_counter(socket_int s) : s_(s), count_(1) {}
			virtual ~shared_counter() {}
			
			socket_int& socket() { return s_; }
			count_type count() const { return count_; }
			count_type increment() { return ++count_; }
			count_type decrement() { return --count_; }
			
		private:
			socket_int s_;
			count_type count_;
		};
		
		shared_counter* p_;
		
		void decrement() {
			if (p_ != NULL && p_->decrement() == 0) {
				if (p_->socket() >= 0) close_socket(p_->socket());
				delete p_;
				p_ = NULL;
			}
		}
	};
}

#endif // CLX_SOCKET_H
