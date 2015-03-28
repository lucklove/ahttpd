#pragma once

#include <string>
#include <vector>
#include <asio.hpp>
#include <sstream>
#include "header.hh"
#include "package.hh"
#include "ptrs.hh"

class Response : public Package, 
	public std::enable_shared_from_this<Response> {
public:
	enum status_t {
		header_already_send = 0,
		ok = 200, created, accepted, no_content,
		multiple_choices = 300, moved_permanently, moved_temporarily, not_modified,
		bad_request = 400, unauthorized, forbidden = 403, not_found,
		internal_server_error = 500, not_implemented, bad_gateway, service_unavailable 
	};

	Response(Server *server, ConnectionPtr connection) :
		Package(server, connection), status_(ok) {}
	~Response() override;
	void setStatus(status_t status) { status_ = status; }
	short getStatus() { return status_; }
	void flush() { flush(true); }
private:
	void flush(bool chunked, const std::function<void(std::ostream&)>& posttreat = {});
	status_t status_;
};
