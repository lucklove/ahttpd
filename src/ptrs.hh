#pragma once

#include <memory>

namespace boost 
{ 
namespace asio 
{ 
namespace ssl 
{
class context;
}
class io_service;
}
namespace system 
{
class error_code;
}
}

namespace ahttpd 
{
class Connection;
using ConnectionPtr = std::shared_ptr<Connection>;

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

class SslConnection;
using SslConnectionPtr = std::shared_ptr<SslConnection>;

class Package;
using PackagePtr = std::shared_ptr<Package>;

class Request;
using RequestPtr = std::shared_ptr<Request>;

class Response;
using ResponsePtr = std::shared_ptr<Response>;

class MailPkg;
using MailPkgPtr = std::shared_ptr<MailPkg>;

}	/**< namespace ahttpd */
