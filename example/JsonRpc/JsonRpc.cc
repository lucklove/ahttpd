#include "server.hh"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::write_json;
using boost::property_tree::ptree;
using boost::property_tree::read_json;

namespace {

struct map_t {
	const char* func_name;
	void (*function)(ptree& in, ptree& out);
} maps[] = {
};
}

struct JsonRpc : RequestHandler {
	using RequestHandler::RequestHandler;
	
	void handleRequest(RequestPtr req, ResponsePtr rep) override {
		int id = 0;
		try {
			ptree pt;
			ptree ret;
			read_json<ptree>(req->in(), pt);
			id = pt.get<int>("id");
			std::string func_name = pt.get<std::string>("method");
			std::cerr << func_name;
			for(auto f : maps) {
				if(func_name == f.func_name) {
					f.function(pt, ret);
					write_json(rep->out(), ret);
					return;
				}
			}
			rep->out() << "{\"result\": null, \"error\": "
				"\"function not implement\", \"id\": " << pt.get<int>("id") << "}";
		} catch(std::exception& e) {
			rep->out() << "{\"result\": null, \"error\": \"" << e.what() << "\", \"id\": " << id << "}";
		}
	}
};

int 
main(int argc, char* argv[])
{
	try {
		asio::io_service io_service;
		Server server(io_service, "8888");
		server.addHandler("/jsonrpc", new JsonRpc(&server));
		server.run(10);	
	} catch(std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
