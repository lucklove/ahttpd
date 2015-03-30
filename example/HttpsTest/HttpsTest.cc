#include "server.hh"

struct HttpsTest : public RequestHandler {
    using RequestHandler::RequestHandler;
    void handleRequest(RequestPtr req, ResponsePtr rep) override {
        rep->out() << "this should be accessed through HTTPS!" << std::endl;
    }
};

int
main(int argc, char *argv[])
{
    asio::io_service io_service;
    Server server(io_service, "", "9999");	/**< 使用9999监听https报文，并且不打开http端口(第二个参数为"") */
    server.addHandler("/HttpsTest", new HttpsTest(&server));
    server.run();
}
