/* ------------------------------------------------------------------------- */
/*
 *  table.h
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
 *  Last-modified: Thu 30 Aug 2007 11:32:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_TABLE_H
#define CLX_TABLE_H

#include <string>
#include <vector>
#include <istream>
#include <fstream>
#include <stdexcept>

namespace clx {
	namespace detail {
		template <class Container, class Source>
		class parser_base {
		public:
			parser_base() {}
			virtual ~parser_base() {}
			virtual const Container& run(const Source& src) = 0;
		};
	};
	
	/* --------------------------------------------------------------------- */
	//  table
	/* --------------------------------------------------------------------- */
	template <
		class Type,
		class Source = std::string,
		class Container = std::vector<std::vector<Type> >
	>
	class table {
	public:
		typedef Type value_type;
		typedef Source string_type;
		
		typedef Container container;
		typedef typename container::value_type subcontainer;
		typedef typename container::size_type size_type;
		typedef typename container::const_iterator const_iterator;
		typedef typename container::iterator iterator;
		
		// constructor and destructor
		table() : v_(), p_() {}
		
		template <class Functor>
		table(Functor f) :
			v_(), p_(new parser<Functor>(f)) {}
		
		template <class Functor>
		table(const string_type& path, Functor f) :
			v_(), p_(new parser<Functor>(f)) {
			this->read(path);
		}
		
		template <class Ch, class Tr, class Functor>
		explicit table(std::basic_istream<Ch, Tr>& sin, Functor f) :
			v_(), p_(new parser<Functor>(f)) {
			this->read(sin);
		}
		
		virtual ~table() { if (p_) delete p_; }
		
		// operator
		template <class Ch, class Tr>
		bool readline(std::basic_istream<Ch, Tr>& sin) {
			string_type tmp;
			if (!std::getline(sin, tmp)) return false;
			if (p_) v_.insert(v_.end(), p_->run(tmp));
			return true;
		}
		
		template <class Ch, class Tr>
		table& read(std::basic_istream<Ch, Tr>& sin) {
			while (this->readline(sin));
			return *this;
		}
		
		table& read(const string_type& path) {
			std::basic_ifstream<typename string_type::value_type,
				typename string_type::traits_type> fin(path);
			if (!fin.is_open()) throw std::runtime_error("No such file or directory");
			this->read(fin);
			fin.close();
			return *this;
		}
		
		void reset() { v_.clear(); }
		
		// inherit container methods
		const subcontainer& operator[](size_type pos) const { return v_[pos]; }
		const subcontainer& at(size_type pos) const { return v_.at(pos); }
		const subcontainer& front() const { return v_.front(); }
		const subcontainer& back() const { return v_.back(); }
		
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
		typedef detail::parser_base<typename Container::value_type, string_type>* parser_ptr;
		
		container v_;
		parser_ptr p_;
		
		template <class F>
		class parser : public detail::parser_base<typename Container::value_type, string_type> {
		public:
			typedef typename Container::value_type container_type;
			parser(F f) : f_(f) {}
			virtual ~parser() {}
			virtual const container_type& run(const string_type& src) { return f_(src); }
		private:
			F f_;
		};
	};
}

#endif // CLX_TABLE_H
