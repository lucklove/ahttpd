#pragma once

#include "ptrs.hh"

void fcgi(boost::asio::io_service& service, const std::string& host, 
	const std::string& port, RequestPtr req, ResponsePtr res);
