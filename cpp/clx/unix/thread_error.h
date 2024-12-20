/* ------------------------------------------------------------------------- */
/*
 *  unix/thread_error.h
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
 *  Last-modified: Wed 13 Aug 2008 11:53:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UNIX_THREAD_ERROR_H
#define CLX_UNIX_THREAD_ERROR_H

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <stdexcept>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  thread_error
	/* --------------------------------------------------------------------- */
	class thread_error : public std::runtime_error {
	public:
		typedef char char_type;
		typedef std::string string_type;
		typedef std::stringstream sstream_type;
		
		static const int nbuf = 1024;
		
		explicit thread_error(const string_type& what_arg) :
			std::runtime_error(what_arg) {
			errno_ = errno;
		}
		
		virtual ~thread_error() throw() {}
		
		virtual const char_type* what() {
			sstream_type tmp;
			char message[nbuf];
			strerror_r(errno_, message, sizeof(message));
			tmp << std::runtime_error::what() << ": " << message;
			return tmp.str().c_str();
		}
		
	private:
		int errno_;
	};
}

#endif // CLX_THREAD_ERROR_H
