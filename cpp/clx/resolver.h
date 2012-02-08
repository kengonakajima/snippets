/* ------------------------------------------------------------------------- */
/*
 *  resolver.h
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
 *  Last-modified: Fri 01 Feb 2008 12:02:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_RESOLVER_H
#define CLX_RESOLVER_H

#include <string>
#include <vector>
#include "socket.h"
#include "lexical_cast.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_resolver
	/* --------------------------------------------------------------------- */
	template <int Family>
	class basic_resolver {
	public:
		typedef struct in_addr address_type;
		typedef char char_type;
		typedef std::basic_string<char> string_type;
		typedef unsigned int size_type;
		typedef std::vector<address_type> address_list;
		typedef std::vector<string_type> alias_list;
		
		// constructor and destructor
		basic_resolver() : name_(), addr_(), aliases_(), len_() {}
		
		explicit basic_resolver(const char_type* s) :
			name_(), addr_(), aliases_(), len_() { this->lookup(s); }
		
		explicit basic_resolver(const string_type& s) :
			name_(), addr_(), aliases_(), len_() { this->lookup(s); }
		
		virtual ~basic_resolver() {}
		
		basic_resolver& operator=(const char_type* s) {
			this->lookup(s);
			return *this;
		}
		
		basic_resolver& operator=(const string_type& s) {
			this->lookup(s);
			return *this;
		}
		
		// operator
		bool lookup(const char_type* s) {
			address_type addr;
			struct hostent* entry;
			
			if (!::inet_aton(s, &addr)) entry = ::gethostbyname(s);
			else entry = ::gethostbyaddr((const char*)&addr, sizeof(address_type), Family);
			
			if (entry == NULL) return false;
			name_ = entry->h_name;
			len_ = entry->h_length;
			for (int i = 0; entry->h_aliases[i]; ++i) aliases_.push_back(entry->h_aliases[i]);
			for (int i = 0; entry->h_addr_list[i]; ++i) addr_.push_back(*((address_type*)entry->h_addr_list[i]));
			
			return true;
		}
		
		bool lookup(const string_type& s) { return this->lookup(s.c_str()); }
		
		// accessor
		const address_list& addresses() const { return addr_; }
		const string_type& hostname() const { return name_; }
		const alias_list& aliases() const { return aliases_; }
		int length() const { return len_; }
		
	private:
		string_type name_;
		address_list addr_;
		alias_list aliases_;
		int len_;
	};
	
	/* --------------------------------------------------------------------- */
	//  port
	/* --------------------------------------------------------------------- */
	int port(const char* service, const char* protocol) {
		int dest = -1;
		struct servent* serv = ::getservbyname(service, protocol);
		if (serv == NULL) {
			try {
				dest = lexical_cast<int>(service);
			}
			catch (bad_lexical_cast) {
				return -1;
			}
		}
		else dest = ::ntohs(serv->s_port);
		return dest;
	}
	
	int port(const std::basic_string<char>& service, const std::basic_string<char>& protocol) {
		return port(service.c_str(), protocol.c_str());
	}
	
	typedef basic_resolver<AF_INET> resolver;
};

#endif // CLX_RESOLVER_H
