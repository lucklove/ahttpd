#include "TcpConnection.hh"

namespace ahttpd {

TcpConnection::~TcpConnection()
{
}

boost::asio::ip::tcp::socket& TcpConnection::nativeSocket() 
{ 
    return socket_; 
}

void TcpConnection::stop()
{
    std::unique_lock<std::mutex> lck(stop_mutex_);
    if(stoped_)
        return;
    stoped_ = true;
    boost::system::error_code ignored_ec;
    nativeSocket().cancel();
    nativeSocket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    socket_.close(); 
}

bool TcpConnection::stoped()
{ 
    return stoped_; 
}

const char* TcpConnection::type()
{ 
    return "tcp"; 
}
    
void TcpConnection::asyncConnect(const std::string& host, 
        const std::string& port,
        std::function<void(ConnectionPtr)> handler)
{
    boost::asio::ip::tcp::resolver::query query(host, port);
    resolver_.async_resolve(query,
        [=, ptr = shared_from_this()](const boost::system::error_code& err,
            boost::asio::ip::tcp::resolver::iterator endpoint_iterator) 
    {
            if(err) 
            {
                Log("DEBUG") << __FILE__ << ":" << __LINE__;
                Log("ERROR") << err.message();
                Log("DEBUG") << "host: " << host << " port: " << port;
                handler(nullptr);
            } 
            else 
            {
                boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
                nativeSocket().async_connect(endpoint,
                    std::bind(&TcpConnection::handle_connect, this, std::placeholders::_1, 
                        ++endpoint_iterator, handler, ptr));
            }
        }
    );
}        

void TcpConnection::async_read_until(const std::string& delim, 
    std::function<void(const boost::system::error_code &, size_t)> handler)
{
    boost::asio::async_read_until(socket_, readBuffer(), delim, handler);
}
    
void TcpConnection::async_read(
    std::function<size_t(const boost::system::error_code &, size_t)> completion,
    std::function<void(const boost::system::error_code &, size_t)> handler)
{
    boost::asio::async_read(socket_, readBuffer(), completion, handler);
}

void TcpConnection::async_write(const std::string& msg,
    std::function<void(const boost::system::error_code&, size_t)> handler)
{
    boost::asio::async_write(socket_, boost::asio::buffer(msg), handler);
}
}
