/* ------------------------------------------------------------------------- */
/*
 *  tokenmap.h
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
#ifndef CLX_TOKENMAP_H
#define CLX_TOKENMAP_H

#include <string>
#include <map>
#include "tokenizer_func.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_tokenmap
	/* --------------------------------------------------------------------- */
	template <
		class TokenizerFunc,
		class KeyT = char,
		class ValueT = std::basic_string<char>,
		class Container = std::map<KeyT, ValueT>
	>
	class basic_tokenmap {
	public:
		typedef TokenizerFunc functor;
		typedef typename functor::char_type char_type;
		typedef std::basic_string<char_type> string_type;
		
		typedef KeyT key_type;
		typedef ValueT value_type;
		typedef Container container;
		typedef typename container::size_type size_type;
		typedef typename container::const_iterator const_iterator;
		typedef typename container::iterator iterator;
		
		// constructor and destructor
		explicit basic_tokenmap(functor f = functor()) :
			v_(), f_(f) {}
		
		template <class InputIterator>
		explicit basic_tokenmap(InputIterator first, InputIterator last, functor f = functor()) :
			v_(), f_(f) { this->assign(first, last); }
		
		explicit basic_tokenmap(const string_type& s, functor f = functor()) :
			v_(), f_(f) { this->assign(s); }
		
		explicit basic_tokenmap(const char_type* s, functor f = functor()) :
			v_(), f_(f) { this->assign(s); }
		
		virtual ~basic_tokenmap() {}
		
		//operator
		void reset() {
			v_.clear();
			f_.reset();
		}
		
		template <class InputIterator>
		container& assign(InputIterator first, InputIterator last) {
			key_type key;
			value_type val;
			
			this->reset();
			while (f_(first, last, key, val)) {
				v_.insert(std::pair<key_type, value_type>(key, val));
				if (first == last) break;
				++first;
				key = key_type();
				val = value_type();
			}
			return v_;
		}
		
		container& assign(const string_type& s) {
			if (s.empty()) return v_;
			return this->assign(s.begin(), s.end());
		}
		
		container& assign(const char_type* s) {
			string_type tmp(s);
			return this->assign(tmp);
		}
		
		template <class InputIterator>
		container& operator()(InputIterator first, InputIterator last) {
			return this->assign(first, last);
		}
		
		container& operator()(const string_type& s) { return this->assign(s); }
		container& operator()(const char_type* s) { return this->assign(s); }
		
		// inherit container methods
		value_type& operator[](const key_type& key) { return v_[key]; }
		value_type operator[](const key_type& key) const { return v_[key]; }
		
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
		
		const_iterator find(const key_type& x) const { return v_.find(x); }
		iterator find(const key_type& x) { return v_.find(x); }
		bool exist(const key_type& x) const { return v_.find(x) != v_.end(); }
		
		// get container
		container& data() { return v_; }
		const container& data() const { return v_; }
		
	private:
		container v_;
		functor f_;
	};
	
	typedef basic_tokenmap<format_separator<char>, char, std::basic_string<char> > tokenmap;
#ifdef CLX_USE_WCHAR
	typedef basic_tokenmap<format_separator<wchar_t>, wchar_t, std::basic_string<wchar_t> > wtokenmap;
#endif // CLX_USE_WCHAR
	
	/* --------------------------------------------------------------------- */
	//  create_tokenmap
	/* --------------------------------------------------------------------- */
	template <class KeyT, class ValueT, class TokenizerFunc>
	inline basic_tokenmap<TokenizerFunc, KeyT, ValueT> create_tokenmap(TokenizerFunc f) {
		return basic_tokenmap<TokenizerFunc, KeyT, ValueT>(f);
	}
}

#endif // CLX_TOKENMAP_H
