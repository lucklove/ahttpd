#include "net.hh"
#include "TcpConnection.hh"
#include "SslConnection.hh"

namespace ahttpd 
{
namespace 
{
void transfer_data(ConnectionPtr conn1, ConnectionPtr conn2)
{
    if(conn1->stoped() || conn2->stoped())
        return;
    conn1->asyncRead(boost::asio::transfer_at_least(1), 
        [=](const boost::system::error_code& err, size_t n)
        { 
            if(err) 
            {
                conn1->stop();
                conn2->stop();
                return;
            }
            std::stringstream ss; 
            ss << &conn1->readBuffer();
            conn2->asyncWrite(ss.str(), [=](const boost::system::error_code& e, size_t n) 
            {
                if(e) 
                {
                    conn1->stop();
                    conn2->stop();
                }
            });
            transfer_data(conn1, conn2);
        }
    );
}

void 
async_connect(ConnectionPtr conn, const std::string& host,
    const std::string& port, std::function<void(ConnectionPtr)> handler)
{
    conn->asyncConnect(host, port, [=](ConnectionPtr c) 
    {
        if(c) 
        {
            handler(c);
        } 
        else 
        {
            Log("ERROR") << "Connect to " << host << ":" << port << " failed";
            handler(nullptr);
        }
    });
}
}

void TcpConnect(boost::asio::io_service& service, const std::string& host,
    const std::string& port, std::function<void(ConnectionPtr)> handler)
{
    async_connect(std::make_shared<TcpConnection>(service), host, port, handler);
}

void SslConnect(boost::asio::io_service& service, const std::string& host,
    const std::string& port, std::function<void(ConnectionPtr)> handler)
{
    boost::asio::ssl::context ssl_context(boost::asio::ssl::context::sslv23);
    ssl_context.set_default_verify_paths();
    async_connect(std::make_shared<SslConnection>(service, ssl_context), host, port, handler);
}

void tunnel(ConnectionPtr conn1, ConnectionPtr conn2)
{
    transfer_data(conn1, conn2);
    transfer_data(conn2, conn1);
}

}    /**< namespace ahttpd */
