#include "WebServer.hh"
#include "fcgi.hh"
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <regex>
#include <algorithm>
#include <typeinfo>

namespace {
/**
 * \biref 查找index文件，目前仅支持index.html和index.php
 */ 
std::string
find_index(const std::string& path)
{
	if(access((path + "index.html").c_str(), 0) == 0) {
		return "index.html";
	} else if(access((path + "index.php").c_str(), 0) == 0) {
		return "index.php";
	} else {
		return "";
	}
}

/**
 * \brief 判断给定文件是否为目录
 */ 
bool
is_dir(const std::string& path)
{
	if(opendir(path.c_str()) == nullptr)
		return false;
	return true;
}
}

void
WebServer::handleRequest(RequestPtr req, ResponsePtr res) 
{
	std::string file_name = doc_root;
	std::string path = req->getPath();
	std::string* host = req->getHeader("Host");

	if(!host) {
		res->setStatus(400);
		return;
	}

	if(is_dir(doc_root + path) && path[path.size()-1] != '/') {
		res->setStatus(302);
		if(typeid(res->connection()) == typeid(SslConnectionPtr)) {
			res->addHeader("Location", "https://" + *host + path + "/");
		} else {
			res->addHeader("Location", "http://" + *host + path + "/");
		}
		return;
	}

	if(path[path.size()-1] == '/') {
		std::string index = find_index(doc_root + path);
		if(index == "") {
			res->setStatus(403);
			return;
		}
		path += index;
	}

	if(path.rfind(".php") == path.size() - 4) { /**< 如果是php文件 */
		req->setPath(path);
		/** 就交给php-fpm处理 */
		fcgi(server_->service(), "localhost", "9000", doc_root, req, res);
		return;
	}
	/** 否则就直接发送请求的文件 */
	file_name += path;
	Log("DEBUG") << file_name;
	std::ifstream file(file_name);
	if(!file) {
		/** 打不开(无论何种原因导致)都视为不存在 */
		res->setStatus(Response::Not_Found);
		return;
	}
	/** 同股后缀猜Mime */
	std::string mime = guessMimeType(file_name);
	if(mime.find("text") != mime.npos)	/**< 比如text/plain, text/html */
		mime += "; charset='utf-8'";	/**< 这样防止乱码 */
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
			/** 必须给出绝对路径，不然会出错 */
			std::cout << "useage: ./webserver abs_doc_root" << std::endl;
			return 0;
		} else {
			server.addHandler("/", new WebServer(&server, argv[1]));
		}
		server.run(10);						/**< 给io_service 10个线程 */
	} catch(std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}
	return 0;
}
