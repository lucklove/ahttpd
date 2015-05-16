#pragma once

#include <functional>
#include "ptrs.hh"

/**
 * \brief 将http请求报文解析为Request
 * \param req 指向(尚未成型的) Request的shared_ptr.
 * \param handler 
 * 	解析完成后的回调函数
 * 	若解析失败则以nullptr为参数调用handler
 */ 	
void parseRequest(ConnectionPtr conn, std::function<void(RequestPtr)> handler);

/**
 * \brief 将http响应报文解析为Response
 * \param res 指向(尚未成型的) Response的shared_ptr.
 * \param handler 
 * 	解析完成后的回调函数
 * 	若解析失败则以nullptr为参数调用handler
 */ 	
void parseResponse(ConnectionPtr conn, std::function<void(ResponsePtr)> handler);
