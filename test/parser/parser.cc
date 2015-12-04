#include "UnitTest.hh"
#include "client.hh"
#include "server.hh"
#include "TcpConnection.hh"
#include "response.hh"
#include "client.hh"
#include <fstream>

using namespace ahttpd;

struct  ReqHandler : RequestHandler 
{
    void handleRequest(RequestPtr req, ResponsePtr res) 
    {
        /**< do nothing */
    }
};

TEST_CASE(parse_chunked_body_test)
{
    
    std::stringstream config("{\"http port\":\"8888\"}");
    Server s(config);
    auto handler = std::make_shared<ReqHandler>();
    s.addHandler("/", handler.get());
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(s.service());
    conn->asyncConnect("localhost", "8888", [&](ConnectionPtr conn_ptr) 
    {
        TEST_REQUIRE(conn_ptr);
        conn_ptr->asyncWrite("GET / HTTP/1.0\r\nTransfer-encoding: chunked\r\n\r\n", 
            [](const boost::system::error_code& e, size_t n) 
        {
                TEST_REQUIRE(!e);
        });
        conn_ptr->asyncWrite("3\r\n", 
            [](const boost::system::error_code& e, size_t n) 
        {
                TEST_REQUIRE(!e);
        });
        conn_ptr->asyncWrite("123\r\n", 
            [](const boost::system::error_code& e, size_t n) 
        {
                TEST_REQUIRE(!e);
        });
        conn_ptr->asyncWrite("4\r\n", 
            [](const boost::system::error_code& e, size_t n) 
        {
                TEST_REQUIRE(!e);
        });
        conn_ptr->asyncWrite("1234\r\n", 
            [](const boost::system::error_code& e, size_t n) 
        {
                TEST_REQUIRE(!e);
        });
        conn_ptr->asyncWrite("0\r\n\r\n", 
            [](const boost::system::error_code& e, size_t n) 
        {
                TEST_REQUIRE(!e);
        });
        conn_ptr->asyncReadUntil("\n", 
            [=, &s](const boost::system::error_code& e, size_t n) 
        {
                TEST_REQUIRE(!e);
                std::istream in(&conn_ptr->readBuffer());
                std::string first_line;
                getline(in, first_line);
                TEST_CHECK(first_line == "HTTP/1.1 200 OK\r");
                s.stop();
        });
    });
    s.run();
}

TEST_CASE(buffer_over_flow_chunked_body_test)
{
    std::stringstream config("{\"http port\":\"8888\"}");
    Server s(config);
    auto handler = std::make_shared<ReqHandler>();
    s.addHandler("/", handler.get());
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(s.service());
    conn->asyncConnect("localhost", "8888", [&](ConnectionPtr conn_ptr) 
    {
        TEST_REQUIRE(conn_ptr);
        conn_ptr->asyncWrite("GET / HTTP/1.0\r\nTransfer-encoding: chunked\r\n\r\n", 
            [](const boost::system::error_code& e, size_t n) 
        {
                TEST_REQUIRE(!e);
        });
        conn_ptr->asyncWrite("1048577\r\n",
            [](const boost::system::error_code& e, size_t n) 
        {
                TEST_REQUIRE(!e);
        });
        conn_ptr->asyncReadUntil("\n", 
            [=, &s](const boost::system::error_code& e, size_t n) 
        {
                TEST_CHECK(e);
                s.stop();
        });
    });
    s.run();
}

TEST_CASE(abort_chunked_body_test)
{
    
    std::stringstream config("{\"http port\":\"8888\"}");
    Server s(config);
    auto handler = std::make_shared<ReqHandler>();
    s.addHandler("/", handler.get());
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(s.service());
    conn->asyncConnect("localhost", "8888", [&](ConnectionPtr conn_ptr) 
    {
        TEST_REQUIRE(conn_ptr);
        conn_ptr->asyncWrite("GET / HTTP/1.0\r\nTransfer-encoding: chunked\r\n\r\n", 
            [](const boost::system::error_code& e, size_t n) 
        {
                TEST_REQUIRE(!e);
        });
        conn_ptr->asyncWrite("1\r\n", 
            [&s](const boost::system::error_code& e, size_t n) 
        {
            TEST_CHECK(!e);
            s.stop();            /**< abort */
        });
    });
    s.run();
}

TEST_CASE(bad_content_length_test)
{
    
    std::stringstream config("{\"http port\":\"8888\"}");
    Server s(config);
    auto handler = std::make_shared<ReqHandler>();
    s.addHandler("/", handler.get());
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(s.service());
    conn->asyncConnect("localhost", "8888", [&](ConnectionPtr conn_ptr) 
    {
        TEST_REQUIRE(conn_ptr);
        conn_ptr->asyncWrite("GET / HTTP/1.0\r\nContent-Length: not number\r\n\r\n", 
            [](const boost::system::error_code& e, size_t n) 
        {
            TEST_REQUIRE(!e);
        });
        conn_ptr->asyncWrite("1\r\n", 
            [&s](const boost::system::error_code& e, size_t n) 
        {
            TEST_CHECK(!e);
            s.stop();
        });
    });
    s.run();
    Client c(s.service());
    c.request("GET", "localhost:8888", [&](ResponsePtr res) 
    {
        TEST_REQUIRE(res);
        TEST_CHECK(res->getStatus() == 200);            /**< 确认服务器没崩溃 */
        s.stop();
    });
    s.run();
}
