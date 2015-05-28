#include "StaticServer.hh"
#include "fcgi.hh"
#include <unistd.h>
#include <fstream>
#include <regex>
#include <algorithm>

namespace {
std::string
find_index(std::string path)
{
	if(access((path + "index.html").c_str(), 0) == 0) {
		return "index.html";
	} else if(access((path + "index.php").c_str(), 0) == 0) {
		return "index.php";
	} else {
		return "";
	}
}
}

void
StaticServer::handleRequest(RequestPtr req, ResponsePtr res) 
{
	std::string file_name = doc_root;
	std::string path = req->getPath();
	if(path[path.size()-1] == '/')
		path += find_index(doc_root + path);
	if(path.rfind(".php") == path.size() - 4) {
		req->setPath(path);
		fcgi(server_->service(), "localhost", "9000", doc_root + req->getPath(), req, res);
		return;
	}
	file_name += path;
	std::ifstream file(file_name);
	if(!file) {
		res->setStatus(Response::Not_Found);
		return;
	}
	std::string mime = guessMimeType(file_name);
	if(mime == "text/plain")
		mime += "; charset='utf-8'";
	res->setMimeType(mime);
	res->out() << file.rdbuf();
}

int 
main(int argc, char* argv[])
{
	try {
		std::stringstream config("{\"http port\":\"8888\"}");
		Server server(config);		
		if(argc < 2) {
			std::cout << "useage: ./webserver abs_doc_root" << std::endl;
			return 0;
		} else {
			server.addHandler("/", new StaticServer(&server, argv[1]));
		}
		server.run(10);						/**< 给io_service 10个线程 */
	} catch(std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}
	return 0;
}
