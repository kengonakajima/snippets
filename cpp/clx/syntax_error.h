/* ------------------------------------------------------------------------- */
/*
 *  syntax_error.h
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
 *  Last-modified: Fri 16 Mar 2007 17:52:03 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SYNTAX_ERROR_H
#define CLX_SYNTAX_ERROR_H

#include <stdexcept>
#include <string>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  syntax_error
	/* --------------------------------------------------------------------- */
	class syntax_error : public std::runtime_error {
	public:
		typedef unsigned int size_type;
		typedef char char_type;
		typedef std::string string_type;
		
		explicit syntax_error(size_type n, const string_type& what_arg) :
			std::runtime_error(what_arg), line_(n) {}
		
		virtual ~syntax_error() throw() {}
		
		size_type line() { return line_; }
		
	private:
		size_type line_;
	};
}

#endif // CLX_SYNTAX_ERROR_H
