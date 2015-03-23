#pragma once

#include <functional>
#include "request.hh"

/**
 * \brief 将http报文解析为Request
 * \param req 指向(尚未成型的) Request的shared_ptr.
 * \param handler 
 * 	解析完成后的回调函数
 * 	第二个若第二个参数传入true,则表示正常解析，成型的Request通过第一个参数传入
 * 	否则，解析失败
 */ 	
void parseRequest(RequestPtr req, const std::function<void(RequestPtr, bool)>& handler);
