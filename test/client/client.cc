#include <boost/test/unit_test.hpp>
#include "client.hh"
#include "server.hh"
#include "response.hh"

struct SetCookie : RequestHandler {
	void handleRequest(RequestPtr req, ResponsePtr res) {
		for(auto c : cookies)
			res->setCookie(c);	
	}
	std::vector<response_cookie_t> cookies;
};

struct EchoCookie : RequestHandler {
	void handleRequest(RequestPtr req, ResponsePtr res) {
		auto cptr = req->getHeader("Cookie");
		if(cptr)
			res->out() << *cptr;
	}
};

struct EchoBody : RequestHandler {
	void handleRequest(RequestPtr req, ResponsePtr res) {
		BOOST_REQUIRE(req->getMethod() == "POST");
		res->out() << req->in().rdbuf();
	}
};

BOOST_AUTO_TEST_CASE(client_request_test)
{
	Client c;
	c.request("GET", "http://www.example.com",
		[](ResponsePtr res) {
			BOOST_REQUIRE(res);
			BOOST_CHECK(res->getStatus() == Response::Ok);
		}
	);
	c.request("GET", "https://www.example.com",	
		[](ResponsePtr res) {
			BOOST_REQUIRE(res);
			BOOST_CHECK(res->getStatus() == Response::Ok);
		}
	);
	c.apply();
}

BOOST_AUTO_TEST_CASE(client_request_chunked_body_test)
{
	std::stringstream config("{\"http port\":\"8888\"}");
	Server s(config);
	auto echo_body = std::make_shared<EchoBody>();
	s.addHandler("/echo", echo_body.get());
	Client c(s.service());
	c.request("POST", "http://localhost:8888/echo",
		[&](ResponsePtr res) {
			BOOST_REQUIRE(res);
			BOOST_CHECK(res->getStatus() == Response::Ok);
			std::stringstream ss;
			ss << res->in().rdbuf();
			BOOST_CHECK(ss.str() == "this is a chunked body");
			Log("NOTE") << ss.str();
			s.stop();
		},
		[](RequestPtr req) {
			BOOST_REQUIRE(req);
			req->out() << "this ";
			req->flush();
			req->out() << "is ";
			req->flush();
			req->out() << "a ";
			req->flush();
			req->out() << "chunked ";
			req->flush();
			req->out() << "body";
			req->flush();
		}
	);
	s.run();
}

BOOST_AUTO_TEST_CASE(client_cookie_muti_test)
{
	std::stringstream config("{\"http port\":\"8888\"}");
	Server s(config);
	auto set_cookie = std::make_shared<SetCookie>();
	auto echo_cookie = std::make_shared<EchoCookie>();
	s.addHandler("/", set_cookie.get());
	s.addHandler("/echo", echo_cookie.get());
	set_cookie->cookies.push_back(response_cookie_t().setKey("key1").setVal("val1"));
	set_cookie->cookies.push_back(response_cookie_t().setKey("key2").setVal("val2").setMaxAge(10));
	set_cookie->cookies.push_back(response_cookie_t().setKey("key3").setVal("val3").setMaxAge(10));
	Client c(s.service());
	c.enableCookie();
	c.request("GET", "http://localhost:8888", [&](ResponsePtr res) {
		c.request("GET", "http://localhost:8888/echo", [&](ResponsePtr res) {
			std::stringstream ss;
			ss << res->out().rdbuf();
			BOOST_CHECK(ss.str() == "key1=val1; key2=val2; key3=val3");
			Log("NOTE") << ss.str();
			s.stop();
		});
	});
	s.run();
}

BOOST_AUTO_TEST_CASE(client_cookie_expires_test)
{
	std::stringstream config("{\"http port\":\"8888\"}");
	Server s(config);
	auto set_cookie = std::make_shared<SetCookie>();
	auto echo_cookie = std::make_shared<EchoCookie>();
	s.addHandler("/", set_cookie.get());
	s.addHandler("/echo", echo_cookie.get());
	set_cookie->cookies.push_back(response_cookie_t().setKey("key1").setVal("val1").setMaxAge(1));
	set_cookie->cookies.push_back(response_cookie_t().setKey("key2").setVal("val2").setMaxAge(10));
	Client c(s.service());
	c.enableCookie();
	c.request("GET", "http://localhost:8888", [&](ResponsePtr res) {
		std::this_thread::sleep_for(std::chrono::seconds(2));
		c.request("GET", "http://localhost:8888/echo", [&](ResponsePtr res) {
			std::stringstream ss;
			ss << res->out().rdbuf();
			BOOST_CHECK(ss.str() == "key2=val2");
			Log("NOTE") << ss.str();
			s.stop();
		});
	});
	s.run();
}

BOOST_AUTO_TEST_CASE(client_cookie_path_test)
{
	std::stringstream config("{\"http port\":\"8888\"}");
	Server s(config);
	auto set_cookie = std::make_shared<SetCookie>();
	auto echo_cookie = std::make_shared<EchoCookie>();
	s.addHandler("/", set_cookie.get());
	s.addHandler("/echo", echo_cookie.get());
	set_cookie->cookies.push_back(response_cookie_t().setKey("key1").setVal("val1").setPath("/echo").setMaxAge(10));
	set_cookie->cookies.push_back(response_cookie_t().setKey("key2").setVal("val2").setPath("/no_such_path").setMaxAge(10));
	set_cookie->cookies.push_back(response_cookie_t().setKey("key3").setVal("val3").setPath("/").setMaxAge(10));
	set_cookie->cookies.push_back(response_cookie_t().setKey("key4").setVal("val4").setPath("/echobad").setMaxAge(10));
	set_cookie->cookies.push_back(response_cookie_t().setKey("key5").setVal("val5").setPath("/echo/").setMaxAge(10));
	Client c(s.service());
	c.enableCookie();
	c.request("GET", "http://localhost:8888", [&](ResponsePtr res) {
		c.request("GET", "http://localhost:8888/echo", [&](ResponsePtr res) {
			std::stringstream ss;
			ss << res->out().rdbuf();
			BOOST_CHECK(ss.str() == "key1=val1; key3=val3; key5=val5");
			Log("NOTE") << ss.str();
			s.stop();
		});
	});
	s.run();
}

BOOST_AUTO_TEST_CASE(client_cookie_domain_test)
{
	std::stringstream config("{\"http port\":\"8888\"}");
	Server s(config);
	auto set_cookie = std::make_shared<SetCookie>();
	auto echo_cookie = std::make_shared<EchoCookie>();
	s.addHandler("/", set_cookie.get());
	s.addHandler("/echo", echo_cookie.get());
	set_cookie->cookies.push_back(response_cookie_t().setKey("key1").setVal("val1").setDomain("127.0.0.1").setMaxAge(10));
	set_cookie->cookies.push_back(response_cookie_t().setKey("key2").setVal("val2").setDomain(".localhost").setMaxAge(10));
	set_cookie->cookies.push_back(response_cookie_t().setKey("key3").setVal("val3").setMaxAge(10));
	set_cookie->cookies.push_back(response_cookie_t().setKey("key4").setVal("val4").setDomain("host").setMaxAge(10));
	Client c(s.service());
	c.enableCookie();
	c.request("GET", "http://localhost:8888", [&](ResponsePtr res) {
		c.request("GET", "http://localhost:8888/echo", [&](ResponsePtr res) {
			std::stringstream ss;
			ss << res->out().rdbuf();
			BOOST_CHECK(ss.str() == "key2=val2; key3=val3");
			Log("NOTE") << ss.str();
			s.stop();
		});
	});
	s.run();
}
