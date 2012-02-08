/* ------------------------------------------------------------------------- */
/*
 *  ini.h
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
 *  Last-modified: Fri 17 Oct 2008 07:19:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_INI_H
#define CLX_INI_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stdexcept>
#include "literal.h"
#include "lexical_cast.h"
#include "salgorithm.h"
#include "exception.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  ini_container
	/* --------------------------------------------------------------------- */
	template <
		class Type,
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class ini_container {
	public:
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		
		typedef std::map<string_type, Type> container;
		typedef typename container::size_type size_type;
		typedef typename container::key_type key_type;
		typedef typename container::mapped_type mapped_type;
		typedef typename container::value_type value_type;
		typedef typename container::iterator iterator;
		typedef typename container::const_iterator const_iterator;
		
		ini_container() : v_() {}
		
		bool insert(const key_type& s) {
			std::vector<string_type> v;
			split_if(s, v, is_any_of(LITERAL("=")));
			if (v.empty()) return false;
			
			string_type key = v.at(0);
			string_type val;
			if (v.size() >= 2) val = v.at(1);
			
			value_type elem(key, lexical_cast<mapped_type>(val));
			std::pair<iterator, bool> ret = v_.insert(elem);
			return ret.second;
		}
		
		// inherit container methods
		mapped_type& operator[](const key_type& key) { return v_[key]; }
		
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
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_ini
	/* --------------------------------------------------------------------- */
	template <
		class Type,
		class CharT = char,
		class Traits = std::char_traits<CharT>
	>
	class basic_ini {
	public:
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		
		typedef string_type key_type;
		typedef Type value_type;
		
		typedef ini_container<Type, CharT, Traits> subcontainer;
		typedef typename subcontainer::iterator sub_iterator;
		typedef typename subcontainer::const_iterator const_sub_iterator;
		
		typedef std::map<string_type, subcontainer> container;
		typedef typename container::size_type size_type;
		typedef typename container::iterator iterator;
		typedef typename container::const_iterator const_iterator;
		
		basic_ini() : v_() {}
		
		template <class Ch, class Tr>
		basic_ini(std::basic_istream<Ch, Tr>& sin) : v_() {
			this->read(sin);
		}
		
		explicit basic_ini(const string_type& path) : v_() {
			this->read(path);
		}
		
		explicit basic_ini(const char_type* path) : v_() {
			this->read(path);
		}
		
		virtual ~basic_ini() {}
		
		template <class Ch, class Tr>
		basic_ini& read(std::basic_istream<Ch, Tr>& sin) {
			const char_type semi = LITERAL(';');
			const char_type lb = LITERAL('[');
			const char_type rb = LITERAL(']');
			
			string_type cur;
			size_t n = 0;
			
			string_type tmp;
			while (std::getline(sin, tmp)) {
				chomp(tmp);
				++n;
				
				if (tmp.empty() || tmp.at(0) == semi) continue;
				else if (tmp.at(0) == lb) { // section
					size_t last = tmp.find(rb);
					if (last == string_type::npos) throw syntax_error(n, "expected ']' token");
					cur = tmp.substr(1, last - 1);
					this->insert(cur);
				}
				else { // parser and insert the string "key=value"
					if (cur.empty()) throw syntax_error(n, "expected section name");
					if (!v_[cur].insert(tmp)) throw syntax_error(n, "unknown error");
				}
			}
			
			return *this;
		}
		
		basic_ini& read(const char_type* path) {
			std::basic_ifstream<CharT, Traits> fs(path);
			if (fs.fail()) {
				std::basic_stringstream<CharT, Traits> msg;
				msg << path << ": no such file or directory";
				throw std::runtime_error(msg.str());
			}
			return this->read(fs);
		}
		
		basic_ini& read(const string_type& path) {
			if (path.empty()) return *this;
			return this->read(path.c_str());
		}
		
		// make new section
		bool insert(const key_type& section) {
			std::pair<iterator, bool> ret;
			std::pair<key_type, subcontainer> elem(section, subcontainer());
			ret = v_.insert(elem);
			return ret.second;
		}
		
		// inherit container methods
		subcontainer& operator[](const key_type& key) { return v_[key]; }
		
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
		
		const_iterator find(const key_type& section) const { return v_.find(section); }
		iterator find(const key_type& section) { return v_.find(section); }
		
		bool exist(const key_type& section) const { return v_.find(section) != v_.end(); }
		
		bool exist(const key_type& section, const key_type& key) const {
			const_iterator p = v_.find(section);
			if (p == v_.end()) return false;
			return p->second.exist(key);
		}
		
		// get container
		container& data() { return v_; }
		const container& data() const { return v_; }
		
	private:
		container v_;
	};
	
	typedef basic_ini<std::basic_string<char> > ini;
#ifdef CLX_USE_WCHAR
	typedef basic_ini<std::basic_string<wchar_t> > wini;
#endif // CLX_USE_WCHAR
}

#endif // CLX_INI_H
