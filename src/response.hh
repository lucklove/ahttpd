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
private:
	std::string version_;
	status_t status_;
	std::string msg_;
};
