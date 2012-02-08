/* ------------------------------------------------------------------------- */
/*
 *  json_separator.h
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
 *  Last-modified: Mon 25 Aug 2008 21:37:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_JSON_SEPARATOR_H
#define CLX_JSON_SEPARATOR_H

#include <cctype>
#include <stdexcept>
#include "predicate.h"
#include "lexical_cast.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  json_control_charset
	/* --------------------------------------------------------------------- */
	template <class CharT>
	struct json_control_charset {
		typedef CharT char_type;
		
		char_type escape;
		char_type quot;
		char_type comma;
		char_type colon;
		char_type lbrace;
		char_type rbrace;
		char_type lbracket;
		char_type rbracket;
		
		char_type backspace;
		char_type ff;
		char_type lf;
		char_type cr;
		char_type tab;
		char_type unicode;
		
		json_control_charset() :
			escape(LITERAL('\\')), quot(LITERAL('"')), comma(LITERAL(',')), colon(LITERAL(':')),
			lbrace(LITERAL('{')), rbrace(LITERAL('}')),
			lbracket(LITERAL('[')), rbracket(LITERAL(']')),
			backspace(LITERAL('b')), ff(LITERAL('f')),lf(LITERAL('n')), cr(LITERAL('r')),
			tab(LITERAL('t')), unicode(LITERAL('u')) {}
	};
	
	namespace detail {
		/* ----------------------------------------------------------------- */
		//  unicode
		/* ----------------------------------------------------------------- */
		template <typename CharT>
		struct unicode {
			static std::basic_string<CharT> convert(int code) {
				return std::basic_string<CharT>(1, static_cast<CharT>(code));
			}
		};
		
		template <>
		struct unicode<char> {
			static std::basic_string<char> convert(int code) {
				std::basic_string<char> dest;
				
				if(code < 0x0080) { // character 0x0000 - 0x007f...
					dest.push_back(0x00 | ((code & 0x007f) >> 0));
				}
				else if(code < 0x0800) { // character 0x0080 - 0x07ff...
					dest.push_back(0xc0 | ((code & 0x07c0) >> 6));
					dest.push_back(0x80 | ((code & 0x003f) >> 0));
				}
				else { // character 0x0800 - 0xffff...
					dest.push_back(0xe0 | ((code & 0x00f000) >> 12));
					dest.push_back(0x80 | ((code & 0x000fc0) >>  6));
					dest.push_back(0x80 | ((code & 0x00003f) >>  0));
				}
				
				return dest;
				//throw std::runtime_error("convert unicode to acii is not supported");
			}
		};
		
		/* ----------------------------------------------------------------- */
		//  next_string
		/* ----------------------------------------------------------------- */
		template <class InputIterator, class CharT>
		bool next_string(InputIterator& next, InputIterator last, std::basic_string<CharT>& dest) {
			json_control_charset<CharT> ctrl;
			
			++next;
			while (next != last && *next != ctrl.quot) {
				if (*next == ctrl.escape) {
					++next;
					if (*next == ctrl.unicode) {
						++next;
						std::basic_string<CharT> code;
						for (int i = 0; i < 3; ++i, ++next) code += *next;
						code += *next;
						dest += detail::unicode<CharT>::convert(lexical_cast<int>(code, std::ios::hex));
					}
					else if (*next == ctrl.backspace) dest += LITERAL('\b');
					else if (*next == ctrl.cr) dest += LITERAL('\r');
					else if (*next == ctrl.lf) dest += LITERAL('\n');
					else if (*next == ctrl.ff) dest += LITERAL('\f');
					else if (*next == ctrl.tab) dest += LITERAL('\t');
					else dest += *next;
				}
				else dest += *next;
				++next;
			}
			
			return next != last;
		}
		
		/* ----------------------------------------------------------------- */
		//  next_member
		/* ----------------------------------------------------------------- */
		template <class InputIterator, class CharT>
		bool next_member(InputIterator& next, InputIterator last,
			std::basic_string<CharT>& dest, CharT beg_char, CharT end_char) {
			int n = 1;
			while (next != last && n > 0) {
				dest += *next;
				++next;
				if (*next == beg_char) ++n;
				else if (*next == end_char) --n;
			}
			
			if (next != last) dest += *next;
			return next != last;
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  json_separator
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class json_separator {
	public:
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		
		void reset() {}
		
		template <class InputIterator, class Key, class Token>
		bool operator()(InputIterator& next, InputIterator last, Key& key, Token& dest) {
			if (next == last || (*next != ctrl.lbrace && *next != ctrl.quot)) return false;
			else if (*next == ctrl.lbrace) ++next;
			
			while (next != last && *next != ctrl.colon) {
				if (*next == ctrl.quot) {
					if (!detail::next_string(next, last, key)) return false;
				}
				else if (!is_space()(*next)) key += *next;
				++next;
			}
			++next;
			
			string_type tmp;
			while (next != last) {
				if (*next == ctrl.comma || *next == ctrl.rbrace) {
					if (!tmp.empty()) dest = lexical_cast<Token>(tmp);
					return true;
				}
				else if (*next == ctrl.lbracket) {
					if (!detail::next_member(next, last, tmp, ctrl.lbracket, ctrl.rbracket)) break;
				}
				else if (*next == ctrl.lbrace) {
					if (!detail::next_member(next, last, tmp, ctrl.lbrace, ctrl.rbrace)) break;
				}
				else if (*next == ctrl.quot) {
					if (!detail::next_string(next, last, tmp)) break;
				}
				else if (!is_space()(*next)) tmp += *next;
				++next;
			}
			
			return false;
		}
		
	private:
		json_control_charset<CharT> ctrl;
	};
}

#endif // CLX_JSON_SEPARATOR_H
