#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include "package.hh"
#include "header.hh"
#include "cookie.hh"
#include "log.hh"

/**
 * \breif 包含http client的请求信息
 */
class Request : public Package, 
	public std::enable_shared_from_this<Request> {
public :
	using Package::Package;
	~Request() override;
			
	/**
 	 * \brief 获取请求的path
 	 * \return path
 	 */ 
	std::string getPath() { return path_; }

 	/**
	 * \brief 设置path
 	 */ 
	void setPath(const std::string& path) { path_ = path; }


	/**
 	 * \brief 获取请求中的query string.
 	 * \return query string.
 	 */
	std::string getQueryString() { return query_; }

 	/** 
	 * \brief 设置请求中的query string.
 	 * \return query string.
 	 */
	void setQueryString(const std::string& query) { query_ = query; }
 
 
	/**
 	 * \brief 获取请求方法
 	 * \return method
 	 */ 
	std::string getMethod() { return method_; }

	/**
 	 * \brief 设置请求方法
 	 */ 
	void setMethod(const std::string& method) { method_ = method; }

	/**
 	 * \brief 获取http版本HTTP/1.0 HTTP1.1
 	 * \return 版本信息
 	 */  
	std::string getVersion() override { return version_; }
	
	/**
 	 * \brief 设置http版本HTTP/1.0 HTTP1.1
 	 */  
	void setVersion(const std::string& version) { version_ = version; }

	void setCookie(const request_cookie_t& cookie) {
		std::string header_val = cookie.key;
		if(cookie.val != "")
			header_val += "=" + cookie.val;

		std::string *h = getHeader("Cookie");
		if(h) {
			*h += "; " + header_val;
		} else {
			addHeader("Cookie", header_val);
		}
	}

	const std::string* getCookieValue(const std::string& key) {
		for(auto& rc : cookie_jar_) {
			if(rc.key == key)
				return &rc.val;
		}
		return nullptr;
	}

	const std::vector<request_cookie_t>& cookieJar() {
		return cookie_jar_;
	}

	void parseCookie() override {
		std::string* cookie_header = getHeader("Cookie");
		if(cookie_header) {
			cookie_jar_ = parseRequestCookie(*cookie_header);
		}
	}

	void flush();
	void basicAuth(const std::string& auth);
	std::string basicAuthInfo();
	std::string proxyAuthInfo();

private:
	std::string method_;
	std::string path_;
	std::string query_;
	std::string version_;
	std::vector<request_cookie_t> cookie_jar_;	
};
