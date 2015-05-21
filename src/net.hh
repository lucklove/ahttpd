#pragma once

#include <string>
#include <functional>
#include "ptrs.hh"

void TcpConnect(boost::asio::io_service& service, const std::string& host,
	const std::string& port, std::function<void(ConnectionPtr)> handler);

void SslConnect(boost::asio::io_service& service, const std::string& host,
	const std::string& port, std::function<void(ConnectionPtr)> handler);

void tunnel(ConnectionPtr conn1, ConnectionPtr conn2);
