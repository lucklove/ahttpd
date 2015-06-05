#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "header.hh"
#include "package.hh"
#include "ptrs.hh"

namespace ahttpd {

class Response : public Package, 
	public std::enable_shared_from_this<Response> {
public:
	typedef unsigned short status_t;
	enum {
		Continue = 100, Switching, Processing,
		Ok = 200, Created, Accepted, Non_Authoritative_Information, 
			No_Content, Reset_Content, Partial_Content, Muti_Status,
		Multiple_Choices = 300, Moved_Permanently, Moved_Temporarily, See_Other, Not_Modified, 
		Bad_Request = 400, Unauthorized, Payment_Required, Forbidden, Not_Found,
		Internal_Server_Error = 500, Not_Implemented, Bad_Gateway, Service_Unavailable 
	};

	Response(ConnectionPtr connection) :
		Package(connection), status_(Ok) {}
	~Response() override;

	void setMimeType(const std::string& mime) { 
		delHeader("Content-Type");
		addHeader("Content-Type", mime);
	}
	std::string getVersion() { return version_; }
	void setVersion(const std::string& version) { version_ = version; }
	status_t getStatus() { return status_; }
	void setStatus(status_t status) { status_ = status; }
	std::string getMessage() { return msg_; }
	void setMessage(const std::string& msg) { msg_ = msg; }
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
	
private:
	std::string version_;
	status_t status_;
	std::string msg_;
	std::vector<response_cookie_t> cookie_jar_;
	void parseCookie() {
		cookie_jar_ = parseResponseCookie(getHeaders("Set-Cookie"));		
	}
	friend void parseResponse(ConnectionPtr, std::function<void(ResponsePtr)>);
};

}	/**< namespace ahttpd */
