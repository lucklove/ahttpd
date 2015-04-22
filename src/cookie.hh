#pragma once

#include <string>
#include <vector>
#include "utils.hh"

class Cookie {
public:
	Cookie(const std::string& token, time_t expires = 0, const std::string& domain = "", 
		const std::string& path = "/", const std::string& secure = "") : 
		token_(token), expires_(expires), domain_(domain), path_(path), secure_(secure)
	{}
	
	std::string getToken() {
		return token_;
	}
	std::string getExpires() {
		return gmtTime(expires_);
	}
	std::string getDomain() {
		return domain_;
	}
	std::string getPath() {
		return path_;
	}
	std::string getSecure() {
		return secure_;
	}
private:
	std::string token_;
	time_t expires_;
	std::string domain_;
	std::string path_;
	std::string secure_;
};

class CookieJar {
public:
	void putCookie(const Cookie& c) {
		cookies_.push_back(c);
	}
	const Cookie* getCookie(const std::string& name) {
		for(auto& c : cookies_) {
			if(c.getToken().size() > name.size() && c.getToken().substr(0, name.size()) == name)
				return &c;
		}
		return nullptr;
	}
	const std::vector<Cookie>& cookieMap() {
		return cookies_;
	}
private:
	std::vector<Cookie> cookies_;	
};
