/* ------------------------------------------------------------------------- */
/*
 *  smtp.h
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
#ifndef CLX_SMTP_H
#define CLX_SMTP_H

#include <iostream>
#include <string>
#include <deque>
#include <stdexcept>
#include "tcp.h"
#include "base64.h"
#include "md5.h"
#include "hmac.h"
#include "lexical_cast.h"
#include "strip.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  smtp_error
	/* --------------------------------------------------------------------- */
	class smtp_error : public std::runtime_error {
	public:
		typedef char char_type;
		typedef std::string string_type;
		
		explicit smtp_error(int code, const string_type& what_arg) :
			std::runtime_error(what_arg), code_(code) {}
		
		virtual ~smtp_error() throw() {}
		
		int code() const { return code_; }
		
	private:
		int code_;
	};
	
	/* --------------------------------------------------------------------- */
	//  smtp_auth
	/* --------------------------------------------------------------------- */
	namespace smtp_auth {
		enum { none = 0x01, plain = 0x02, login = 0x04, cram_md5 = 0x08, best = 0x0f };
	}
	
	/* --------------------------------------------------------------------- */
	//  basic_smtp
	/* --------------------------------------------------------------------- */
	template <class Socket>
	class basic_smtp : public Socket {
	public:
		typedef Socket socket_type;
		typedef char char_type;
		typedef std::basic_string<char_type> string_type;
		typedef std::pair<int, string_type> response_type;
		typedef std::deque<response_type> response_array;
		
		basic_smtp() : socket_type(), host_(), port_(0), auth_(0), res_() {}
		
		basic_smtp(const basic_smtp& cp) :
			socket_type(cp), host_(cp.host_), port_(cp.port_), auth_(0), res_(cp.res_) {}
		
		explicit basic_smtp(const string_type& host, int port = 25) :
			socket_type(), host_(host), port_(port), auth_(0), res_() {
			try {
				this->start();
			}
			catch (socket_error& e) {
				(*this) = basic_smtp();
				throw e;
			}
			catch (std::runtime_error& e) {
				(*this) = basic_smtp();
				throw e;
			}
		}
		
		explicit basic_smtp(const char_type* host, int port = 25) :
			socket_type(), host_(host), port_(port), auth_(0), res_() {
			try {
				this->start();
			}
			catch (socket_error& e) {
				(*this) = basic_smtp();
				throw e;
			}
			catch (std::runtime_error& e) {
				(*this) = basic_smtp();
				throw e;
			}
		}
		
		virtual ~basic_smtp() throw() { this->finish(); }
		
		basic_smtp& start() {
			if (!this->is_open()) this->connect(host_, port_);
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			this->xresponse(ss);
			return *this;
		}
		
		basic_smtp& start(const string_type& host, int port = 25) {
			host_ = host;
			port_ = port;
			return this->start();
		}
		
		basic_smtp& start(const char_type* host, int port = 25) {
			string_type tmp(host);
			return this->start(tmp, port);
		}
		
		void finish() {
			if (this->is_open()) {
				basic_sockbuf<socket_type, char_type> sbuf(*this);
				std::basic_iostream<char_type> ss(&sbuf);
				ss << "QUIT\r\n";
				this->xresponse(ss);
				this->close();
			}
		}
		
		basic_smtp& login(const string_type& id) {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			ss << ("EHLO " + id + " " + host_ + "\r\n");
			this->xresponse(ss);
			
			for (size_t i = 0; i < res_.size(); ++i) {
				if (res_[i].first == 250 && res_[i].second.find("AUTH") != string_type::npos) {
					if (res_[i].second.find("PLAIN") != string_type::npos) auth_ |= smtp_auth::plain;
					if (res_[i].second.find("LOGIN") != string_type::npos) auth_ |= smtp_auth::login;
					if (res_[i].second.find("CRAM-MD5") != string_type::npos) auth_ |= smtp_auth::cram_md5;
				}
			}
			
			return *this;
		}
		
		basic_smtp& login(const char_type* id) {
			string_type tmp(id);
			return this->login(tmp);
		}
		
		basic_smtp& login(const string_type& id, const string_type& pass, int type = smtp_auth::best) {
			this->login(id);
			
			if ((type & auth_ & smtp_auth::cram_md5) != 0) return xcram_md5(id, pass);
			else if ((type & auth_ & smtp_auth::plain) != 0) return xplain(id, pass);
			else if ((type & auth_ & smtp_auth::login) != 0) return xlogin(id, pass);
			else if ((type & smtp_auth::none) != 0) return *this;
			
			throw smtp_error(502, "SMTP-AUTH not supported");
			return *this;
		}
		
		basic_smtp& login(const char_type* id, const char_type* pass, int type = smtp_auth::best) {
			string_type tmp_id(id);
			string_type tmp_pass(pass);
			return this->login(tmp_id, tmp_pass, type);
		}
		
		basic_smtp& mail(const string_type& from, const string_type& to, const string_type& message) {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			string_type buf;
			
			// A mail transaction has 4 steps.
			buf = "MAIL FROM:<" + from + ">\r\n";
			ss << buf;
			this->xresponse(ss);
			
			buf = "RCPT TO:<" + to + ">\r\n";
			ss << buf;
			this->xresponse(ss);
			
			ss << "DATA\r\n";
			this->xresponse(ss);
			
			ss << message;
			this->xresponse(ss);
			
			return *this;
		}
		
		basic_smtp& mail(const char_type* from, const char_type* to, const char_type* message) {
			string_type tmp_from(from);
			string_type tmp_to(to);
			string_type tmp_msg(message);
			return this->mail(tmp_from, tmp_to, tmp_msg);
		}
		
		// get response messages from the server
		response_array& responses() { return res_; }
		const response_array& responses() const { return res_; }
		const response_type& response(size_t index) { return res_.at(index); }
		
	private:
		string_type host_;
		int port_;
		int auth_;
		response_array res_;
		
		template <class InputStream>
		void xresponse(InputStream& sin) {
			string_type buf;
			while (std::getline(sin, buf)) {
				chomp(buf);
				response_type tmp;
				
				try {
					tmp.first = lexical_cast<int>(buf.substr(0, 3));
				}
				catch (clx::bad_lexical_cast&) {
					string_type msg = "invalid response: " + buf;
					throw std::runtime_error(msg);
				}
				
				tmp.second = buf.substr(4);
				res_.push_front(tmp);
				if (tmp.first >= 400) throw smtp_error(tmp.first, tmp.second);
				if (buf[3] == ' ') break;
			}
		}
		
		basic_smtp& xplain(const string_type& id, const string_type& pass) {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			
			int len = id.size() * 2 + pass.size() + 2;
			char* p = new char[len + 1];
			std::memset(p, 0, len + 1);
			std::memcpy(p, id.c_str(), id.size());
			std::memcpy(&p[id.size() + 1], id.c_str(), id.size());
			std::memcpy(&p[(id.size() + 1) * 2], pass.c_str(), pass.size());
			
			ss << ("AUTH PLAIN " + base64::encode(p, len) + "\r\n");
			delete [] p;
			
			this->xresponse(ss);
			return *this;
		}
		
		basic_smtp& xlogin(const string_type& id, const string_type& pass) {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			
			ss << "AUTH LOGIN\r\n";
			this->xresponse(ss);
			ss << (clx::base64::encode(id) + "\r\n");
			this->xresponse(ss);
			ss << (clx::base64::encode(pass) + "\r\n");
			this->xresponse(ss);
			
			return *this;
		}
		
		basic_smtp& xcram_md5(const string_type& id, const string_type& pass) {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			
			ss << "AUTH CRAM-MD5\r\n";
			this->xresponse(ss);
			
			string_type buf = base64::decode(this->response(0).second);
			buf = id + " " + hmac<md5>(pass, buf).to_string();
			ss << (base64::encode(buf) + "\r\n");
			this->xresponse(ss);
			
			return *this;
		}
	};
	
	typedef basic_smtp<tcp::socket> smtp;
}

#endif // CLX_SMTP_H
