#include "server.hh"
#include <fstream>

struct HttpsTest : public RequestHandler {
    void handleRequest(RequestPtr req, ResponsePtr rep) override {
        rep->out() << "this should be accessed through HTTPS!" << std::endl;
    }
};

int
main(int argc, char *argv[])
{
	std::stringstream config("{"
		"\"https port\":\"9999\","
		"\"http port\":\"8888\","
		"\"verify file\":\"../../../certificate/server.csr\","
		"\"certificate chain file\":\"../../../certificate/server.crt\","
		"\"private key\":\"../../../certificate/server.key\","
		"\"tmp dh file\":\"../../../certificate/server.dh\""
	"}");
	Server server(config);
	server.addHandler("/HttpsTest", new HttpsTest());
	server.run();
}
