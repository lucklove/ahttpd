#include <boost/test/unit_test.hpp>
#include "server.hh"
#include "fcgi.hh"
#include "client.hh"

using namespace ahttpd;

struct FcgiTestServer : RequestHandler {
	FcgiTestServer(Server *s) : server_(s) {}
	void handleRequest(RequestPtr req, ResponsePtr res) override {
		fcgi(server_->service(), "localhost", "9000", "/tmp/", req, res);
	}
	Server *server_;
};

BOOST_AUTO_TEST_CASE(fcgi_test)
{
	std::stringstream config("{\"http port\": \"8888\"}");
	Server server(config);
	auto test = std::make_shared<FcgiTestServer>(&server);
	server.addHandler("/", test.get());
	Client c(server.service());
	system("sudo killall php-fpm");	
	c.request("GET", "localhost:8888/no_such_file.php", 
		[&](ResponsePtr res) {
			BOOST_CHECK(res->getStatus() == 500);
			system("sudo php-fpm");
			c.request("GET", "localhost:8888/no_such_file.php", 
				[&](ResponsePtr res) {
					BOOST_CHECK(res->getStatus() == 404);
					server.stop();
				}
			);
		}
	);
	server.run();
}	
