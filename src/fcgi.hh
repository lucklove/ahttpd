#pragma once

#include "ptrs.hh"

void fcgi(boost::asio::io_service& service, const std::string& host, const std::string& port, 
	const std::string& script_path, RequestPtr req, ResponsePtr res);
