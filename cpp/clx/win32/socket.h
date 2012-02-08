/* ------------------------------------------------------------------------- */
/*
 *  win32/socket.h
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
 *  Last-modified: Wed 16 Jul 2008 14:38:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_WIN32_SOCKET_H
#define CLX_WIN32_SOCKET_H

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stdexcept>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <io.h>
#include <windows.h>

typedef SOCKET socket_int;
typedef int socklen_type;

// for IP and/or ICMP header
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned int u_int32_t;

/* ------------------------------------------------------------------------- */
/*
 *  inet_aton
 *
 *  WinSock does not have the inet_aton() function.
 */
/* ------------------------------------------------------------------------- */
inline int inet_aton(const char* s, struct in_addr* addr) {
	int tmp = ::inet_addr(s);
	if (tmp != INADDR_NONE || ::strcmp(s, "255.255.255.255") == 0) {
		if (addr) addr->s_addr = tmp;
		return 1;
	}
	return 0;
}

namespace clx {
	/* --------------------------------------------------------------------- */
	//  winsock_init
	/* --------------------------------------------------------------------- */
	class winsock_init {
	public:
		winsock_init() {
			WSADATA dat;
			::WSAStartup(MAKEWORD(2, 0), &dat);
		}
		
		~winsock_init() {
			::WSACleanup();
		}
	private:
		winsock_init(const winsock_init& cp);
		winsock_init& operator=(const winsock_init& cp);
	};
	
	static winsock_init ws_init_;
	
	/* --------------------------------------------------------------------- */
	//  socket_error
	/* --------------------------------------------------------------------- */
	class socket_error : public std::runtime_error {
	public:
		typedef char char_type;
		typedef std::string string_type;
		typedef std::stringstream sstream_type;
		
		static const int nbuf = 1024;
		
		explicit socket_error(const string_type& what_arg) :
			std::runtime_error(what_arg) {
			errno_ = ::WSAGetLastError();
		}
		
		virtual ~socket_error() throw() {}
		
		virtual const char_type* what() {
			sstream_type tmp;
			char message[nbuf];
			strerror_s(message, sizeof(message), errno_);
			tmp << std::runtime_error::what() << ": " << message;
			return tmp.str().c_str();
		}
		
		int code() const { return errno_; }
		
	private:
		int errno_;
	};
	
	/* --------------------------------------------------------------------- */
	//  open_socket
	/* --------------------------------------------------------------------- */
	inline socket_int open_socket(int family, int type, int flag) {
		return ::socket(family, type, flag);
	}
	
	/* --------------------------------------------------------------------- */
	//  close_socket
	/* --------------------------------------------------------------------- */
	inline void close_socket(socket_int s) {
		::closesocket(s);
	}
	
	/* --------------------------------------------------------------------- */
	//  duplicate_socket
	/* --------------------------------------------------------------------- */
	inline void duplicate_socket(socket_int& dest, socket_int src) {
		::DuplicateHandle(::GetCurrentProcess(), (HANDLE)src,
			::GetCurrentProcess(), (LPHANDLE)&dest,
			0, TRUE, DUPLICATE_SAME_ACCESS);
	}
	
	/* --------------------------------------------------------------------- */
	//  assign_sockaddr
	/* --------------------------------------------------------------------- */
	inline void assign_sockaddr(struct sockaddr_in& dest, int family, const in_addr& ipaddr, int port) {
		std::memset(&dest, 0, sizeof(dest));
		dest.sin_family = family;
		dest.sin_port = ::htons(port);
		std::memcpy((char*)&dest.sin_addr, &ipaddr, sizeof(ipaddr));
	}
	
	/* --------------------------------------------------------------------- */
	//  assign_sockaddr
	/* --------------------------------------------------------------------- */
	inline void assign_sockaddr(struct sockaddr_in& dest, int family, int port) {
			std::memset((char*)&dest, 0, sizeof(dest));
			dest.sin_family = family;
			dest.sin_port = ::htons(port);
			dest.sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);
	}
}

#endif // CLX_WIN32_SOCKET_H
