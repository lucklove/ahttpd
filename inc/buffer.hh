#pragma once

#include <boost/asio.hpp>

namespace ahttpd 
{

/**
 * \brief 用于asio接收/发送数据
 */ 
using buffer_t = ::boost::asio::streambuf;

}	/**< namespace ahttpd */
