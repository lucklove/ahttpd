#include "StaticServer.hh"
#include <fstream>
#include <regex>
#include <algorithm>

void
StaticServer::handleRequest(RequestPtr req, ResponsePtr res) 
{
	if(req->getMethod() != "GET") {				/**< 只允许GET请求 */
		res->setStatus(Response::Not_Implemented);
		return;
	}
	std::string file_name = doc_root;
	std::string path = req->getPath();
	if(path[path.size()-1] == '/')
		path += "index.html";
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
		if(argc == 1) {
			server.addHandler("/", new StaticServer());
		} else {
			server.addHandler("/", new StaticServer(argv[1]));
		}
		server.run(10);						/**< 给io_service 10个线程 */
	} catch(std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
