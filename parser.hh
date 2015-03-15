#pragma once

#include <functional>
#include "request.hh"

void parseRequest(RequestPtr req, std::function<void(RequestPtr, bool)> handler);
