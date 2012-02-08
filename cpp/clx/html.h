/* ------------------------------------------------------------------------- */
/*
 *  html.h
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
 *  Last-modified: Wed 13 Aug 2008 14:14:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_HTML_H
#define CLX_HTML_H

#include <algorithm>
#include <map>
#include <string>
#include "literal.h"

namespace clx {
	namespace detail {
		template <
			class CharT,
			class Traits = std::char_traits<CharT>
		>
		class escape_dictionary {
		public:
			typedef CharT char_type;
			typedef std::basic_string<CharT, Traits> string_type;
			typedef typename std::map<CharT, std::basic_string<CharT, Traits> >::iterator iterator;
			
			escape_dictionary() {
				dict_[LITERAL('&')] = LITERAL("&amp;");
				dict_[LITERAL('"')] = LITERAL("&quot;");
				dict_[LITERAL('<')] = LITERAL("&lt;");
				dict_[LITERAL('>')] = LITERAL("&gt;");
				dict_[LITERAL('\'')] = LITERAL("&#39;");
			}
			
			iterator find(char_type c) { return dict_.find(c); }
			iterator begin() { return dict_.begin(); }
			iterator end() { return dict_.end(); }
			
		private:
			std::map<CharT, std::basic_string<CharT, Traits> > dict_;
		};
	};
	
	namespace html {
		/* ----------------------------------------------------------------- */
		//  escape
		/* ----------------------------------------------------------------- */
		template <class CharT, class InputIterator, class OutputIterator>
		inline OutputIterator escape(InputIterator first, InputIterator last, OutputIterator result) {
			typedef clx::detail::escape_dictionary<CharT> dict_type;
			typedef CharT char_type;
			typedef typename dict_type::iterator iterator;
			
			static dict_type dict;
			
			while (first != last) {
				char_type ch = *first++;
				iterator pos = dict.find(ch);
				if (pos != dict.end()) result = std::copy(pos->second.begin(), pos->second.end(), result);
				else *result++ = ch;
			}
			
			return result;
		}
		
		/* ----------------------------------------------------------------- */
		//  escape
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline std::basic_string<Ch, Tr> escape(const std::basic_string<Ch, Tr>& s) {
			std::basic_string<Ch, Tr> tmp;
			if (s.empty()) return tmp;
			
			std::insert_iterator<std::basic_string<Ch, Tr> > pos(tmp, tmp.begin());
			escape<Ch>(s.begin(), s.end(), pos);
			return tmp;
		}
		
		/* ----------------------------------------------------------------- */
		//  escape
		/* ----------------------------------------------------------------- */
		template <class CharT>
		inline std::basic_string<CharT> escape(const CharT* s) {
			std::basic_string<CharT> tmp(s);
			return escape(tmp);
		}
	}
}

#endif // CLX_HTML_H
