#pragma once

#include <string>
#include <sstream>
#include <thread>
#include <iostream>
#include <utility>
#include <exception>
#include <cstdio>
#include "utils.hh"

namespace ahttpd {

/**
 *\brief 
 *	日志记录类，打印日志信息到控制台，信息包括日志级别，
 *	当前时间，当前线程，以及日志内容
 *\example
 *	Log("WARNING") << "something bad happened";
 */	
class Log {
public:
	/**
 	 * \brief ctor
 	 * \type 日志级别
 	 */ 
	explicit Log(const std::string& type) {
		buffer_ << "[\033[35m" << localTime(time(nullptr)) << "\033[0m] " 
			<< "[\033[36m" <<std::this_thread::get_id() << "\033[0m] "
			<< "[\033[37m" << type << "\033[0m] ";
	}

	/**
 	 * \brief 移动构造
 	 */ 
	Log(Log&& log_) { 
		if(log_.buffer_.rdbuf()->in_avail())
			buffer_ << log_.buffer_.rdbuf();
	}

	/**
 	 * \brief 在析构中完成打印
 	 */ 
	~Log() { 
		try {
			if(buffer_.rdbuf()->in_avail())
				std::cout << buffer_.rdbuf() << std::endl; 
		} catch(std::exception& e) {
			fprintf(stderr, "%s\n", e.what());
		}
	}

	/**
 	 * \brief 输出符号重载，向Log类输入信息
 	 * \param val 任意可以通过输出流输出的实例
 	 */ 
	template<typename T>
	auto&& operator<<(T&& val) { return buffer_ << std::forward<T>(val); }
private:
	std::stringstream buffer_;
};

}	/**< namespace ahttpd */
