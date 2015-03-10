#pragma once

#include <functional>
#include "connection.hh"
#include "request.hh"
#include "buffer.hh"

void parseRequest(RequestPtr req, 
	std::function<void(RequestPtr, bool)> handler);

void parseResponse(RequestPtr req, 
	std::function<void(RequestPtr, bool)> handler);
