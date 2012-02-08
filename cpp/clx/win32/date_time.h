/* ------------------------------------------------------------------------- */
/*
 *  win32/date_time.h
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
 *  Last-modified: Thu 04 Sep 2008 22:15:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_WIN32_DATE_TIME_H
#define CLX_WIN32_DATE_TIME_H

#include <locale>
#include <iostream>
#include <sstream>
#include <string>
#include "../replace.h"
#include "../tokenmap.h"
#include "../tokenizer_func.h"
#include "../lexical_cast.h"

namespace clx {
	namespace detail {
		/* ----------------------------------------------------------------- */
		//  xlocaltime
		/* ----------------------------------------------------------------- */
		void xlocaltime(const std::time_t* timep, struct tm* result) {
			::localtime_s(result, timep);
		}
		
		/* ----------------------------------------------------------------- */
		//  xgmtime
		/* ----------------------------------------------------------------- */
		void xgmtime(const std::time_t* timep, struct tm* result) {
			::gmtime_s(result, timep);
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  xstrptime
		 *
		 *  Trivial implementation of strptime.
		 *  The xstrptime provides limited function compared with strptime.
		 */
		/* ----------------------------------------------------------------- */
		void get_from_monthname(const std::basic_string<char>& name, struct tm* dest,
			const std::time_get<char>& tg) {
			std::basic_stringstream<char> ss;
			ss << name;
			std::istreambuf_iterator<char> from(ss);
			std::istreambuf_iterator<char> to;
			std::ios_base::iostate err = std::ios::goodbit;
			tg.get_monthname(from, to, ss, err, dest);
		}
		
		void get_from_weekname(const std::basic_string<char>& name, struct tm* dest,
			const std::time_get<char>& tg) {
			std::basic_stringstream<char> ss;
			ss << name;
			std::istreambuf_iterator<char> from(ss);
			std::istreambuf_iterator<char> to;
			std::ios_base::iostate err = std::ios::goodbit;
			tg.get_weekday(from, to, ss, err, dest);
		}
		
		void xstrptime(const char* src, const char* fmt, struct tm* dest) {
			std::basic_string<char> tmp_fmt(fmt);
			replace(tmp_fmt, std::basic_string<char>("%F"), std::basic_string<char>("%Y-%m-%d"));
			replace(tmp_fmt, std::basic_string<char>("%T"), std::basic_string<char>("%H:%M:%S"));
			
			std::basic_string<char> dateset = "aAbByYmdeHkMSjuw";
			format_separator<char> sep(tmp_fmt, dateset);
			basic_tokenmap<format_separator<char>, char, std::basic_string<char> > date(sep);
			date.assign(src);
			
			// Consider the value of '69 or less as 20XX.
			if (date.exist('y')) {
				int y = lexical_cast<int>(date['y']);
				if (y < 70) dest->tm_year = y;
				else dest->tm_year = y + 100;
			}
			
			if (date.exist('Y')) dest->tm_year = lexical_cast<int>(date['Y']) - 1900;
			if (date.exist('m')) dest->tm_mon  = lexical_cast<int>(date['m']) - 1;
			if (date.exist('d')) dest->tm_mday = lexical_cast<int>(date['d']);
			else if (date.exist('e')) dest->tm_mday = lexical_cast<int>(date['e']);
			if (date.exist('H')) dest->tm_hour = lexical_cast<int>(date['H']);
			else if (date.exist('k')) dest->tm_hour = lexical_cast<int>(date['k']);
			if (date.exist('M')) dest->tm_min  = lexical_cast<int>(date['M']);
			if (date.exist('S')) dest->tm_sec  = lexical_cast<int>(date['S']);
			if (date.exist('j')) dest->tm_yday = lexical_cast<int>(date['j']);
			if (date.exist('u')) dest->tm_wday = lexical_cast<int>(date['u']) - 1;
			else if (date.exist('w')) dest->tm_wday = lexical_cast<int>(date['w']);
			
			// resolve month name
			const std::time_get<char>& tg = std::use_facet<std::time_get<char> >(std::locale());
			if (date.exist('b')) get_from_monthname(date['b'], dest, tg);
			else if (date.exist('B')) get_from_monthname(date['B'], dest, tg);
			
			// resolve week name
			if (date.exist('a')) get_from_weekname(date['a'], dest, tg);
			else if (date.exist('A')) get_from_weekname(date['A'], dest, tg);
			
			std::time_t t = std::mktime(dest);
			xlocaltime(&t, dest);
		}
		
		/* ----------------------------------------------------------------- */
		//  string_to_time
		/* ----------------------------------------------------------------- */
		template <class CharT>
		void string_to_time(const CharT* src, const CharT* fmt, struct tm* dest) {
			xstrptime(src, fmt, dest);
		}
		
#ifdef CLX_USE_WCHAR
		template <>
		void string_to_time<wchar_t>(const wchar_t* src, const wchar_t* fmt, struct tm* dest) {
			std::basic_string<char> tmp_src = clx::code_convert<char>(src);
			std::basic_string<char> tmp_fmt = clx::code_convert<char>(fmt);
			xstrptime(tmp_src.c_str(), tmp_fmt.c_str(), dest);
		}
#endif // CLX_USE_WCHAR
	}
}

#endif // CLX_WIN32_DATE_TIME_H
