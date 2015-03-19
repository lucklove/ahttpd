#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include "package.hh"
#include "header.hh"

class Request : public Package, 
	public std::enable_shared_from_this<Request> {
public :
	using Package::Package;
	~Request() override {};
	void deliverSelf();

	std::string getUri() { return uri; }
	std::string getMethod() { return method; }
	std::string getVersion() { return version; }
	
	void setUri(const std::string& u) { uri = u; }
	void setMethod(const std::string& m) { method = m; }
	void setVersion(const std::string& v) { version = v; }

private:
	std::string method;
	std::string uri;
	std::string version;
};

using RequestPtr = std::shared_ptr<Request>;
