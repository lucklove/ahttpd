#pragma once

#include <functional>
#include "ptrs.hh"

namespace ahttpd 
{
/**
 * \brief 将http请求报文解析为Request
 * \param conn Connection的shared_ptr.
 * \param handler 
 * 	解析完成后的回调函数
 * 	若解析失败则以nullptr为参数调用handler
 */ 	
void parseRequest(ConnectionPtr conn, std::function<void(RequestPtr)> handler);

/**
 * \brief 将http响应报文解析为Response
 * \param conn Connection的shared_ptr.
 * \param handler 
 * 	解析完成后的回调函数
 * 	若解析失败则以nullptr为参数调用handler
 */ 	
void parseResponse(ConnectionPtr conn, std::function<void(ResponsePtr)> handler);

}	/**< namespace ahttpd */
