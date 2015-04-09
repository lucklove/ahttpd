#pragma once

#include <string>
#include <vector>
#include <asio.hpp>
#include <sstream>
#include <utility>
#include <cstring>
#include "header.hh"
#include "ptrs.hh"

#include "log.hh"

class Server;

class Package
{
public:
	Package(ConnectionPtr connection) :
		connection_(connection) {}

	virtual ~Package() {};

	std::istream& in() { return body; }
	std::ostream& out() { return body; }
	ConnectionPtr connection() { return connection_; }
	void discardConnection() { connection_.reset(); }

	std::vector<std::string> getHeaders(std::string h_name) {
		std::vector<std::string> dst_header;
		for(auto h : headers) {
			if(strcasecmp(h.name.c_str(), h_name.c_str()) == 0)
				dst_header.push_back(h.value);
		}
		return dst_header;
	}

	std::string* getHeader(std::string h_name) {
		for(auto& h : headers) {
			if(strcasecmp(h.name.c_str(), h_name.c_str()) == 0)
				return &h.value;
		}
		return nullptr;
	}

	void addHeader(const std::string& h_name, const std::string& h_value) {
		headers.push_back(header_t{h_name, h_value});
	}

	void setHeader(const std::string h_name, const std::string& h_value) {
		delHeader(h_name);
		addHeader(h_name, h_value);
	}

	void delHeader(const std::string& h_name) {
		for(std::vector<header_t>::iterator it = headers.begin(); it != headers.end(); ++it) {
			if(strcasecmp(it->name.c_str(), h_name.c_str()) == 0)
				headers.erase(it);
		}
	}

	std::vector<header_t>& headerMap() { return headers; }

	bool keepAlive() {		
		std::string* connection_opt = getHeader("Connection");
		if(connection_opt) {
			if(strcasecmp(connection_opt->c_str(), "Keep-alive") == 0) {
				return true;
			} else {
				return false;
			}
		}
		if(version() == "HTTP/1.1")
			return true; 
		return false;
	}

	virtual std::string& version() = 0;

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
	void setHeadLine(const std::string& headline) { headline_ = headline; }
	bool chunked() { return chunked_; }
	void setChunked() { chunked_ = true; }
	void flushPackage();

private:
	bool chunked_ = false;
	bool send_started_ = false;
	std::vector<header_t> headers;
	std::stringstream body;
	ConnectionPtr connection_;
	std::string headline_;
};
