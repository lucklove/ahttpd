#include "UnitTest.hh"
#include <boost/asio.hpp>
#include <mutex>
#include <queue>
#include "connection.hh"
#include "buffer.hh"
#include "net.hh"
#include "log.hh"

using namespace ahttpd;

TEST_CASE(tcp_connect_test)
{
    boost::asio::io_service service;
    TcpConnect(service, "www.example.com", "80",
        [](ConnectionPtr conn) {
            TEST_REQUIRE(conn);
        }
    );
    TcpConnect(service, "localhost", "12345",
        [](ConnectionPtr conn) {
            TEST_CHECK(!conn);
        }
    );
    service.run();
}

TEST_CASE(ssl_connect_test)
{
    boost::asio::io_service service;
    SslConnect(service, "www.example.com", "443",
        [](ConnectionPtr conn) {
            TEST_REQUIRE(conn);
        }
    );
    SslConnect(service, "localhost", "12345",
        [](ConnectionPtr conn) {
            TEST_CHECK(!conn);
        }
    );
    service.run();
}

struct MockConnection : Connection {
    std::queue<std::string> send_queue;
    std::queue<std::string> recv_queue;
    bool stoped_{};
    void stop() override { stoped_ = true; }
    bool stoped() override { return stoped_; }
    const char* type() override { return "test"; }    

    void asyncConnect(const std::string& host, const std::string& port,
        std::function<void(ConnectionPtr)> handler) override {
        assert(false);
    }

    void async_read_until(const std::string& delim, 
        std::function<void(const boost::system::error_code &, size_t)> handler) override {
        assert(false);
    }

    void async_read(std::function<size_t(const boost::system::error_code &, size_t)> completion,
        std::function<void(const boost::system::error_code &, size_t)> handler) override {
        if(send_queue.size() == 0) {
            handler(boost::asio::error::broken_pipe, 0);
            return;
        }
        std::ostream out(&readBuffer());
        out << send_queue.front();        
        send_queue.pop();
        handler({}, 1);
    }

    void async_write(const std::string& msg, 
        std::function<void(const boost::system::error_code&, size_t)> handler) override {
        if(recv_queue.size() == 0) {
            handler(boost::asio::error::broken_pipe, 0);
            return;
        }
        TEST_CHECK(msg == recv_queue.front());
        recv_queue.pop();
        handler({}, 1);
    }
};

TEST_CASE(tunnel_test)
{
    const char *queue[] = { "queue1", "queue2", "queue3", "queue4" };

    std::shared_ptr<MockConnection> conn1 = std::make_shared<MockConnection>();
    std::shared_ptr<MockConnection> conn2 = std::make_shared<MockConnection>();
    for(size_t i = 0; i < sizeof(queue) / sizeof(queue[0]); ++i) {
        conn1->send_queue.push(queue[i]);
        conn2->recv_queue.push(queue[i]);
    }
    tunnel(conn1, conn2);
}
