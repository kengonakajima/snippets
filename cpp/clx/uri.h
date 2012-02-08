/* ------------------------------------------------------------------------- */
/*
 *  uri.h
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
 *  Last-modified: Tue 06 Nov 2007 02:17:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_URI_H
#define CLX_URI_H

#include <string>
#include <sstream>
#include "literal.h"
#include "lexical_cast.h"
#include "predicate.h"

namespace clx {
	namespace detail {
		/* ----------------------------------------------------------------- */
		//  is_reserved
		/* ----------------------------------------------------------------- */
		template <class CharT>
		inline bool is_reserved(CharT c) {
			std::basic_string<CharT> ctrls(LITERAL("!#$&'()*+,-./:;=?@_~"));
			if (is_alnum()(c) || is_any_of(ctrls.c_str())(c)) return true;
			return false;
		}
	};
	
	namespace uri {
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline std::basic_string<Ch, Tr> encode(const std::basic_string<Ch, Tr>& s) {
			Ch ws = LITERAL(' ');
			Ch plus = LITERAL('+');
			Ch percent = LITERAL('%');
			
			std::basic_ostringstream<Ch, Tr> ss;
			for (unsigned int i = 0; i < s.size(); ++i) {
				if (s.at(i) == ws) ss << plus;
				else if (!clx::detail::is_reserved(s.at(i))) {
					ss << percent << std::hex << static_cast<unsigned int>(s.at(i));
				}
				else ss << s.at(i);
			}
			return ss.str();
		}
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		template <class CharT>
		inline std::basic_string<CharT> encode(const CharT* s) {
			std::basic_string<CharT> tmp(s);
			return encode(tmp);
		}
		
		/* ----------------------------------------------------------------- */
		//  decode
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline std::basic_string<Ch, Tr> decode(const std::basic_string<Ch, Tr>& s) {
			Ch percent = LITERAL('%');
			
			std::basic_ostringstream<Ch, Tr> ss;
			for (unsigned int i = 0; i < s.size(); ++i) {
				if (s.at(i) == percent) {
					unsigned int c = lexical_cast<unsigned int>(s.substr(i + 1, 2), std::ios::hex);
					ss << static_cast<Ch>(c);
					i += 2;
				}
				else ss << s.at(i);
			}
			return ss.str();
		}
		
		/* ----------------------------------------------------------------- */
		//  decode
		/* ----------------------------------------------------------------- */
		template <class CharT>
		inline std::basic_string<CharT> decode(const CharT* s) {
			std::basic_string<CharT> tmp(s);
			return decode(tmp);
		}
	};
}

#endif
