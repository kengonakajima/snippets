/* ------------------------------------------------------------------------- */
/*
 *  tokenizer.h
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
 *  Last-modified: Thu 09 Oct 2008 06:44:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_TOKENIZER_H
#define CLX_TOKENIZER_H

#include <string>
#include <vector>
#include "tokenizer_func.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_tokenizer
	/* --------------------------------------------------------------------- */
	template <
		class TokenizerFunc,
		class Type = std::basic_string<char>,
		class Container = std::vector<Type>
	>
	class basic_tokenizer {
	public:
		typedef TokenizerFunc functor;
		typedef typename functor::char_type char_type;
		typedef std::basic_string<char_type> string_type;
		
		typedef Type value_type;
		typedef Container container;
		typedef typename container::size_type size_type;
		typedef typename container::const_iterator const_iterator;
		typedef typename container::iterator iterator;
		
		// constructor and destructor
		explicit basic_tokenizer(functor f = functor()) :
			v_(), f_(f) {}
		
		template <class InputIterator>
		explicit basic_tokenizer(InputIterator first, InputIterator last, functor f = functor()) :
			v_(), f_(f) { this->assign(first, last); }
		
		explicit basic_tokenizer(const string_type& s, functor f = functor()) :
			v_(), f_(f) { this->assign(s); }
		
		explicit basic_tokenizer(const char_type* s, functor f = functor()) :
			v_(), f_(f) { this->assign(s); }
		
		virtual ~basic_tokenizer() {}
		
		//operator
		void reset() {
			v_.clear();
			f_.reset();
		}
		
		template <class InputIterator>
		const container& assign(InputIterator first, InputIterator last) {
			value_type token;
			
			this->reset();
			while (f_(first, last, token)) {
				v_.insert(v_.end(), token);
				if (first == last) break;
				++first;
				token = value_type();
			}
			return v_;
		}
		
		const container& assign(const string_type& s) {
			if (s.empty()) return v_;
			return this->assign(s.begin(), s.end());
		}
		
		const container& assign(const char_type* s) {
			string_type tmp(s);
			return this->assign(tmp);
		}
		
		template <class InputIterator>
		const container& operator()(InputIterator first, InputIterator last) {
			return this->assign(first, last);
		}
		
		const container& operator()(const string_type& s) { return this->assign(s); }
		const container& operator()(const char_type* s) { return this->assign(s); }
		
		// inherit container methods
		const value_type& operator[](size_type pos) const { return v_[pos]; }
		const value_type& at(size_type pos) const { return v_.at(pos); }
		const value_type& front() const { return v_.front(); }
		const value_type& back() const { return v_.back(); }
		
		bool empty() const { return v_.empty(); }
		size_type size() const { return v_.size(); }
		
		const_iterator begin() const { return v_.begin(); }
		const_iterator end() const { return v_.end(); }
		const_iterator rbegin() const { return v_.rbegin(); }
		const_iterator rend() const { return v_.rend(); }
		
		iterator begin() { return v_.begin(); }
		iterator end() { return v_.end(); }
		iterator rbegin() { return v_.rbegin(); }
		iterator rend() { return v_.rend(); }
		
		// get container
		const container& data() const { return v_; }
		
	private:
		container v_;
		functor f_;
	};
	
	typedef basic_tokenizer<char_separator<char>, std::basic_string<char> > tokenizer;
	typedef basic_tokenizer<escape_separator<char>, std::basic_string<char> > csv;
	typedef basic_tokenizer<format_separator<char>, std::basic_string<char> > strftokenizer;
#ifdef CLX_USE_WCHAR
	typedef basic_tokenizer<char_separator<wchar_t>, std::basic_string<wchar_t> > wtokenizer;
	typedef basic_tokenizer<escape_separator<wchar_t>, std::basic_string<wchar_t> > wcsv;
	typedef basic_tokenizer<format_separator<wchar_t>, std::basic_string<wchar_t> > wstrftokenizer;
#endif // CLX_USE_WCHAR
	
	/* --------------------------------------------------------------------- */
	/*
	 *  create_tokenizer
	 *
	 *  The function helps for creating a tokenizer object. This
	 *  function is mainly used when using the tokenizer as functor
	 *  class.
	 */
	/* --------------------------------------------------------------------- */
	template <class Type, class TokenizerFunc>
	inline basic_tokenizer<TokenizerFunc, Type> create_tokenizer(TokenizerFunc f) {
		return basic_tokenizer<TokenizerFunc, Type>(f);
	}
}

#endif // CLX_TOKENIZER_H
