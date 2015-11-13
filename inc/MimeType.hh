#pragma once

#include <string>

namespace ahttpd 
{

/**
 * \brief 通过一个文件路径或url路径判断其对应的mime类型
 */
std::string guessMimeType(const std::string& path);

}	/**< namespace ahttpd */
