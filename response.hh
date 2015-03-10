#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <sstream>
#include "header.hh"

//#include "connection.hh"

/// A response to be sent to a client.
class Response : public Package, 
	public std::enable_shared_from_this<Response> {
public:
	using Package::Package;

	void setStatus(short status) { status_ = status; }
	short getStatus() { return status_; }
private:
	short status_;
		
//	Server *server_;
//	ConnectionPtr connection_;	
};

 using ResponsePtr = std::shared_ptr<Response>;
