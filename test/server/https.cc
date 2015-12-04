#include "ahttpd.hh"
#include "UnitTest.hh"

using namespace ahttpd;

TEST_CASE(https_test)
{
    std::stringstream config("{"
        "\"https port\":\"9999\","
        "\"verify file\":\"certificate/server.csr\","
        "\"certificate chain file\":\"certificate/server.crt\","
        "\"private key\":\"certificate/server.key\","
        "\"tmp dh file\":\"certificate/server.dh\""
    "}");
    Server server(config);
    Client c{server.service()};
    c.request("GET", "https://localhost:9999/something_not_exist", [&](ResponsePtr res) 
    {
        TEST_REQUIRE(res);
        TEST_CHECK(res->getStatus() == Response::Not_Found);
        server.stop();
    });
    server.run();
}
