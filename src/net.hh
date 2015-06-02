#pragma once

#include <string>
#include <functional>
#include "ptrs.hh"

/**
 * \biref TcpConnection的工具函数，用于发起tcp连接并生成ConnectionPtr
 * \param service io_service的引用
 * \param host 对端ip地址或域名
 * \param port 对端端口
 * \param handler 处理函数，接收产生的ConnectionPtr
 */ 
void TcpConnect(boost::asio::io_service& service, const std::string& host,
	const std::string& port, std::function<void(ConnectionPtr)> handler);

/**
 * \biref SslConnection的工具函数，用于发起ssl连接并生成ConnectionPtr
 * \param service io_service的引用
 * \param host 对端ip地址或域名
 * \param port 对端端口
 * \param handler 处理函数，接收产生的ConnectionPtr
 */ 
void SslConnect(boost::asio::io_service& service, const std::string& host,
	const std::string& port, std::function<void(ConnectionPtr)> handler);

/**
 * \brief 隧道，将两个连接对接在一起，使它们的对端可以直接通讯
 * \param conn1 第一个连接
 * \param conn2 第二个连接
 */ 
void tunnel(ConnectionPtr conn1, ConnectionPtr conn2);
