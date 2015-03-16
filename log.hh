#pragma once

#include <string>
#include <sstream>
#include <iostream>

class Log {
public:
	explicit Log(const std::string& type) {
		buffer_ << "[\033[35m" << local_time() << "\033[0m] " 
			<< "[\033[36m" << type << "\033[0m] ";
	}
	Log(const Log&) = delete;
	Log(Log&& log_) { 
		if(log_.buffer_.rdbuf()->in_avail())
			buffer_ << log_.buffer_.rdbuf();
	}
	~Log() { 
		if(buffer_.rdbuf()->in_avail())
			std::cout << buffer_.rdbuf() << std::endl; 
	}
	template<typename T>
	auto&& operator<<(T&& val) { return buffer_ << val; }
private:
	std::string local_time();	
	std::stringstream buffer_;
};
