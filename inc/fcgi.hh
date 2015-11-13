#pragma once

#include "ptrs.hh"
#include <functional>

namespace ahttpd 
{

void fcgi(boost::asio::io_service& service, 
	const std::string& host, 
	const std::string& port, 
	std::string doc_root, 
	RequestPtr req, 
	ResponsePtr res,
	std::function<void(void)> handler = [] {});

}	/**< namespace ahttpd */
