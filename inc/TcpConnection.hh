#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <mutex>
#include <boost/asio.hpp>
#include "connection.hh"

namespace ahttpd 
{

class Server;

class TcpConnection : public Connection 
{
public:
    explicit TcpConnection(boost::asio::io_service& service)
          : socket_(service), resolver_(service)
    {}

    ~TcpConnection() override;

    void stop() override; 

    /**
      * \note 非线程安全
      */ 
    bool stoped() override;

    const char* type() override;
    
    void asyncConnect(const std::string& host, const std::string& port,
        std::function<void(ConnectionPtr)> handler) override;
 
    virtual boost::asio::ip::tcp::socket& nativeSocket();

protected:
    void async_read_until(const std::string& delim, 
        std::function<void(const boost::system::error_code &, size_t)> handler) override;

    void async_read(
        std::function<size_t(const boost::system::error_code &, size_t)> completion,
        std::function<void(const boost::system::error_code &, size_t)> handler) override; 

    void async_write(const std::string& msg,
        std::function<void(const boost::system::error_code&, size_t)> handler) override;

private:
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;
    bool stoped_{};
    std::mutex stop_mutex_;
    void handle_connect(const boost::system::error_code& err, 
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator, 
        std::function<void(ConnectionPtr)> handler,
        ConnectionPtr ptr)                  /**< 防止过早的析构 */ 
    {            
        if(!err) 
        {
            handler(ptr);
        } 
        else if(endpoint_iterator != boost::asio::ip::tcp::resolver::iterator()) 
        {
            nativeSocket().close();
            boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
            nativeSocket().async_connect(endpoint, std::bind(&TcpConnection::handle_connect, this,
                        std::placeholders::_1, ++endpoint_iterator, handler, ptr));
        } 
        else 
        {
            handler(nullptr);
        }
    }
};

}    /**< namespace ahttpd */
