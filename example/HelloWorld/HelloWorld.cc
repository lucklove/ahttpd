#include "ahttpd.hh"

struct HelloWorldHandler : public RequestHandler {
	void handleRequest(RequestPtr req, ResponsePtr rep) override {
		rep->out() << "hello world!" << std::endl;
	}
};

int
main(int argc, char *argv[])
{
	std::stringstream config("{\"http port\":\"8888\"}");
	Server server(config);
	server.addHandler("/HelloWorld", new HelloWorldHandler());
	server.run(10);
}
