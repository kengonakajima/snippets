/* ------------------------------------------------------------------------- */
/*
 *  base64.h
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
 *  Last-modified: Wed 29 Oct 2008 15:29:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_BASE64_H
#define CLX_BASE64_H

#include <iterator>
#include <cstring>
#include <string>
#include <stdexcept>

namespace clx {
	namespace base64 {
		const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		template <class InputIterator, class OutputIterator>
		inline OutputIterator encode(InputIterator first, InputIterator last, OutputIterator result) {
			char tmp[3];
			int i = 0;
			while (first != last) {
				std::memset(tmp, 0, sizeof(tmp));
				for (i = 0; i < 3; ++i) {
					tmp[i] = *first;
					if (++first == last) break;
				}
				if (first == last && i < 2) break;
				
				*result++ = table[(tmp[0] & 0xfc) >> 2];
				*result++ = table[((tmp[0] & 0x03) << 4) | ((tmp[1] & 0xf0) >> 4)];
				*result++ = table[((tmp[1] & 0x0f) << 2) | ((tmp[2] & 0xc0) >> 6)];
				*result++ = table[(tmp[2] & 0x3f)];
			}
			
			switch (i % 3) {
			case 0:
				*result++ = table[(tmp[0] & 0xfc) >> 2];
				*result++ = table[((tmp[0] & 0x03) << 4) | ((tmp[1] & 0xf0) >> 4)];
				*result++ = table[64];
				*result++ = table[64];
				break;
			case 1:
				*result++ = table[(tmp[0] & 0xfc) >> 2];
				*result++ = table[((tmp[0] & 0x03) << 4) | ((tmp[1] & 0xf0) >> 4)];
				*result++ = table[((tmp[1] & 0x0f) << 2) | ((tmp[2] & 0xc0) >> 6)];
				*result++ = table[64];
				break;
			default:
				break;
			}
			
			return result;
		}
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		inline std::string encode(const char* src, size_t n) {
			std::string dest;
			std::insert_iterator<std::string> output(dest, dest.begin());
			encode((char*)src, (char*)(src + n), output);
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		inline std::string encode(const std::string& src) {
			return encode(src.c_str(), src.size());
		}
		
		/* ----------------------------------------------------------------- */
		//  decode
		/* ----------------------------------------------------------------- */
		template <class InputIterator, class OutputIterator>
		inline OutputIterator decode(InputIterator first, InputIterator last, OutputIterator result) {
			while (first != last) {
				char tmp[4];
				std::memset(tmp, 0, sizeof(tmp));
				for (int i = 0; i < 4; ++i) {
					if (first == last) throw std::runtime_error("wrong base64 encoded data");
					if (*first != table[64]) {
						int j = 0;
						while (table[j] != '\0' && table[j] != *first) ++j;
						if (table[i] == '\0') throw std::runtime_error("wrong base64 encoded data");
						tmp[i] = static_cast<char>(j);
					}
					++first;
				}
				
				*result++ = ((tmp[0] << 2) & 0xfc) | ((tmp[1] >> 4) & 0x03);
				*result++ = ((tmp[1] << 4) & 0xf0) | ((tmp[2] >> 2) & 0x0f);
				*result++ = ((tmp[2] << 6) & 0xc0) | (tmp[3] & 0x3f);
			}
			
			return result;
		}
		
		/* ----------------------------------------------------------------- */
		//  decode
		/* ----------------------------------------------------------------- */
		inline std::string decode(const char* src, size_t n) {
			std::string tmp;
			std::insert_iterator<std::string> output(tmp, tmp.begin());
			decode((char*)src, (char*)(src + n), output);
			return std::string(tmp.c_str());
		}
		
		/* ----------------------------------------------------------------- */
		//  decode
		/* ----------------------------------------------------------------- */
		inline std::string decode(const std::string& src) {
			return decode(src.c_str(), src.size());
		}
	}
}

#endif // CLX_BASE64_H
