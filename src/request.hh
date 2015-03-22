#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include "package.hh"
#include "header.hh"

/**
 * \breif 包含http client的请求信息
 */
class Request : public Package, 
	public std::enable_shared_from_this<Request> {
public :
	using Package::Package;
	~Request() override {};
	void deliverSelf();

	/**
 	 * \brief 获取请求的uri
 	 * \return uri
 	 */ 
	std::string getUri() { return uri; }

	/**
 	 * \brief 获取请求方法
 	 * \return method
 	 */ 
	std::string getMethod() { return method; }

	/**
 	 * \brief 获取http版本HTTP/1.0 HTTP1.1
 	 * \return 版本信息
 	 */  
	std::string getVersion() { return version; }
	
	void setUri(const std::string& u) { uri = u; }
	void setMethod(const std::string& m) { method = m; }
	void setVersion(const std::string& v) { version = v; }

private:
	std::string method;
	std::string uri;
	std::string version;
};
