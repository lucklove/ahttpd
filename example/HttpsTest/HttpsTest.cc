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
	std::ifstream config("../../../example/HttpsTest/config.txt");
	if(!config) {
		Log("ERROR") << "Unable to load config file";
		return -1;
	}
	Server server(config);
	server.addHandler("/HttpsTest", new HttpsTest());
	server.run();
}
