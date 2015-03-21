#pragma once

#include <memory>

class Connection;
using ConnectionPtr = std::shared_ptr<Connection>;

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

class SslConnection;
using SslConnectionPtr = std::shared_ptr<SslConnection>;

class Request;
using RequestPtr = std::shared_ptr<Request>;

class Response;
using ResponsePtr = std::shared_ptr<Response>;
