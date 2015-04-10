#pragma once

#include <string>

struct Base64 {
	static std::string encode(const std::string& str);
	static std::string decode(const std::string& str);
};
