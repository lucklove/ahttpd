#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <cstring>
#include "header.hh"
#include "ptrs.hh"
#include "cookie.hh"

namespace ahttpd {

class Server;

class Package
{
public:
	Package(ConnectionPtr connection) :
		connection_(connection) {}

	virtual ~Package() =default;

	std::istream& in() { return body; }
	std::ostream& out() { return body; }
	ConnectionPtr connection() { return connection_; }

	void discardConnection() { connection_.reset(); }

	std::vector<std::string> getHeaders(std::string h_name) {
		std::vector<std::string> dst_header;
		for(auto h : headers_) {
			if(strcasecmp(h.name.c_str(), h_name.c_str()) == 0)
				dst_header.push_back(h.value);
		}
		return dst_header;
	}

	std::string* getHeader(std::string h_name) {
		for(auto& h : headers_) {
			if(strcasecmp(h.name.c_str(), h_name.c_str()) == 0)
				return &h.value;
		}
		return nullptr;
	}

	void addHeader(const std::string& h_name, const std::string& h_value) {
		headers_.push_back(header_t{h_name, h_value});
	}

	void setHeader(const std::string h_name, const std::string& h_value) {
		delHeader(h_name);
		addHeader(h_name, h_value);
	}

	void delHeader(const std::string& h_name) {
		for(std::vector<header_t>::iterator it = headers_.begin(); it != headers_.end();) {
			if(strcasecmp(it->name.c_str(), h_name.c_str()) == 0) {
				it = headers_.erase(it);
			} else {
				++it;
			}
		}
	}

	std::vector<header_t>& getHeaderMap() { return headers_; }

	size_t
	contentLength()
	{
		auto cur = body.tellg();
		body.seekg(0, std::ios_base::end);
		auto res = body.tellg() - cur;
		body.seekg(cur, std::ios_base::beg);
		return res;
	}
protected:

	bool chunked() { return chunked_; }
	void setChunked() { chunked_ = true; }
	void flushPackage();

private:
	bool chunked_ = false;
	bool send_started_ = false;
	std::vector<header_t> headers_;
	std::stringstream body;
	ConnectionPtr connection_;
};

}	/**< namespace ahttpd */
