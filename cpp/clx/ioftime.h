/* ------------------------------------------------------------------------- */
/*
 *  ioftime.h
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
 *  Last-modified: Thu 28 Aug 2008 15:40:02 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_IOFTIME_H
#define CLX_IOFTIME_H

#include <ctime>
#include <string>
#include <stdexcept>
#include <ostream>
#include "date_time.h"
#include "code_convert.h"

namespace clx {
	namespace detail {
		/* ----------------------------------------------------------------- */
		//  string_to_date
		/* ----------------------------------------------------------------- */
		template <class CharT>
		CharT* string_to_date(const CharT* src, const CharT* fmt, struct tm* dest) {
			return ::strptime(src, fmt, dest);
		}
		
#ifdef CLX_USE_WCHAR
		template <>
		wchar_t* string_to_date(const wchar*t src, const wchar_t* fmt, struct tm* dest) {
			std::basic_string<char> tmp_src = clx::code_convert<char>(src);
			std::basic_string<char> tmp_fmt = clx::code_convert<char>(fmt);
			return ::strptime(tmp_src.c_str(), tmp_fmt.c_str(), dest);
		}
#endif // CLX_USE_WCHAR
		
		/* ----------------------------------------------------------------- */
		//  date_to_string
		/* ----------------------------------------------------------------- */
		template <class CharT>
		int date_to_string(CharT* dest, size_t n, const CharT* fmt, const struct tm* date) {
			return std::strftime(dest, n, fmt, date);
		}
		
#ifdef CLX_USE_WCHAR
		template <>
		int date_to_string<wchar_t>(wchar_t* dest, size_t n, const wchar_t* fmt, const struct tm* date) {
			return std::wcsftime(dest, n, fmt, date);
		}
#endif // CLX_USE_WCHAR
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_iftime
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_iftime {
	public:
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		typedef date_time date_type;
		typedef typename date_type::tm_type tm_type;
		
		// constructor and destructor
		basic_iftime() : date_() {}
		
		explicit basic_iftime(const char_type* s, const char_type* fmt) :
			date_() { this->assign(s, fmt); }
		
		explicit basic_iftime(const string_type& s, const string_type& fmt) :
			date_() { this->assign(s, fmt); }
		
		virtual ~basic_iftime() {}
		
		// operator
		const date_type& assign(const char_type* s, const char_type* fmt) {
			tm_type tmp;
			char_type* p = detail::string_to_date(s, fmt, &tmp);
			if (*p != '\0') throw std::runtime_error("string does not match the format");
			date_ = tmp;
			return date_;
		}
		
		const date_type& assign(const string_type& s, const string_type& fmt) {
			return this->assign(s.c_str(), fmt.c_str());
		}
		
		// accessor
		const date_type& data() const { return date_; }
		
	private:
		date_type date_;
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_oftime
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_oftime {
	public:
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		typedef date_time date_type;
		typedef typename date_type::tm_type tm_type;
		
		// constructor and destructor
		basic_oftime() : date_(), str_() {}
		
		explicit basic_oftime(const char_type* fmt) :
			date_(), str_() {
			this->assign(fmt);
		}
		
		template <class Date>
		explicit basic_oftime(const char_type* fmt, const Date& t) :
			date_(t), str_() {
			this->assign(fmt);
		}
		
		explicit basic_oftime(const string_type& fmt) :
			date_(), str_() {
			this->assign(fmt);
		}
		
		template <class Date>
		explicit basic_oftime(const string_type& fmt, const Date& t) :
			date_(t), str_() {
			this->assign(fmt);
		}
		
		template <class Date>
		basic_oftime& operator=(const Date& t) {
			date_ = t;
			return *this;
		}
		
		virtual ~basic_oftime() {}
		
		// operator
		virtual void reset() {
			str_.clear();
		}
		
		const string_type& assign(const char_type* fmt) {
			char_type tmp[1024];
			int n = detail::date_to_string<char_type>(tmp, sizeof(tmp), fmt, date_.data());
			if (n == 0) throw std::runtime_error("temporary buffer is overflow");
			str_ = tmp;
			return str_;
		}
		
		template <class Date>
		const string_type& assign(const char_type* fmt, const Date& t) {
			date_ = t;
			return this->assign(fmt);
		}
		
		const string_type& assign(const string_type& fmt) {
			return this->assign(fmt.c_str());
		}
		
		template <class Date>
		const string_type& assign(const string_type& fmt, const Date& t) {
			date_ = t;
			return this->assign(fmt);
		}
		
		// accessor
		const string_type& str() const { return str_; }
		
		template <class Ch, class Tr>
		friend std::basic_ostream<Ch, Tr>& operator<<(
			std::basic_ostream<Ch, Tr>& sout, const basic_oftime<Ch, Tr>& f) {
			sout << f.str_;
			return sout;
		}
		
	private:
		date_type date_;
		string_type str_;
	};
	
	typedef basic_iftime<char> iftime;
	typedef basic_oftime<char> oftime;
#ifdef CLX_USE_WCHAR
	typedef basic_iftime<wchar_t> wiftime;
	typedef basic_oftime<wchar_t> woftime;
#endif // CLX_USE_WCHAR
}

#endif // CXL_IOFTIME_H
