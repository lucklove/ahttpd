#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "header.hh"
#include "package.hh"
#include "ptrs.hh"

class Response : public Package, 
	public std::enable_shared_from_this<Response> {
public:
	enum status_t {
		ok = 200, created, accepted, non_authoritative_information, 
			no_content, reset_content, partial_content, muti_status,
		multiple_choices = 300, moved_permanently, moved_temporarily, not_modified,
		bad_request = 400, unauthorized, forbidden = 403, not_found,
		internal_server_error = 500, not_implemented, bad_gateway, service_unavailable 
	};

	Response(ConnectionPtr connection) :
		Package(connection), status_(ok) {}
	~Response() override;

	void setMimeType(const std::string& mime) { 
		delHeader("Content-Type");
		addHeader("Content-Type", mime);
	}
	std::string& version() override { return version_; }
	status_t& status() { return status_; }
	std::string& message() { return msg_; }
	void flush();

	void setCookie(const response_cookie_t& cookie) {
		std::string header_val = cookie.key;
		if(cookie.val != "")
			header_val += "=" + cookie.val;
		if(cookie.expires)
			header_val += "; expires=" + gmtTime(cookie.expires) + " GMT";
		if(cookie.domain != "")
			header_val += "; domain=" +  cookie.domain;
		if(cookie.path != "")
			header_val += "; path=" + cookie.path;
		if(cookie.secure)
			header_val += "; secure";
		if(cookie.httponly)
			header_val += "; HttpOnly";
		addHeader("Set-Cookie", header_val);	
	}

	const std::vector<response_cookie_t>& cookieJar() {
		return cookie_jar_;
	}
	
	void parseCookie() override {
		cookie_jar_ = parseResponseCookie(getHeaders("Set-Cookie"));		
	}
private:
	std::string version_;
	status_t status_;
	std::string msg_;
	std::vector<response_cookie_t> cookie_jar_;
};
