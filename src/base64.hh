#pragma once

#include <string>

struct Base64 {
	/**
 	 * \brief base64编码
 	 * \param str待编码字符串
 	 * \return 编码后的字符串
 	 */ 
	static std::string encode(const std::string& str);

	/**
 	 * \brief base64解码
 	 * \param str待解码字符串
 	 * \return 解码后的字符串
 	 */ 
	static std::string decode(const std::string& str);
};
