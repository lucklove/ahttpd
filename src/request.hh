#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include "package.hh"
#include "header.hh"

#include "log.hh"

/**
 * \breif 包含http client的请求信息
 */
class Request : public Package, 
	public std::enable_shared_from_this<Request> {
public :
	using Package::Package;
	~Request() override;
			
	/**
 	 * \brief 获取请求的path
 	 * \return path
 	 */ 
	std::string& path() { return path_; }

	/**
 	 * \brief 获取请求中的query string.
 	 * \return query string.
 	 */
	std::string& query() { return query_; }
 
	/**
 	 * \brief 获取请求方法
 	 * \return method
 	 */ 
	std::string& method() { return method_; }

	/**
 	 * \brief 获取http版本HTTP/1.0 HTTP1.1
 	 * \return 版本信息
 	 */  
	std::string& version() override { return version_; }

	void flush();
	
private:
	std::string method_;
	std::string path_;
	std::string query_;
	std::string version_;
};
