#include "server.hh"

struct HelloWorldHandler : public RequestHandler {
    using RequestHandler::RequestHandler;
    void handleRequest(RequestPtr req, ResponsePtr rep) override {
        rep->out() << "hello world!" << std::endl;
    }
};

int
main(int argc, char *argv[])
{
    Server server("8888");
    server.addHandler("/HelloWorld", new HelloWorldHandler(&server));
    server.run();
}
