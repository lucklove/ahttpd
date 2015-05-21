/********************************************************************************
 *  ahttpd示例程序								*
 *  HTTP代理服务器								*
 *  by Joshua <gnu.crazier@gmail.com> at 2015-5-9				*
 ********************************************************************************/ 	

#include <iostream>
#include "ahttpd.hh"

namespace {
const char* black_list[] = {
	"google.com",
};

bool
in_black_list(const std::string& url)
{
	for(const char *b : black_list) {
		if(url.find(b) != url.npos)
			return true;
	}
	return false;
}
}

struct ProxyHandler : public RequestHandler {
	ProxyHandler(Server& s) : server_(s), client_(s.service()) {}

	void handleRequest(RequestPtr req, ResponsePtr res) override; 
private:
	Server& server_;	
	Client client_;
};

void 
ProxyHandler::handleRequest(RequestPtr req, ResponsePtr res) 
{
//	if(req->proxyAuthInfo() != "luck:luck") {			/**< 用户:密码 */
//		res->setStatus(403);
//		return;
//	}
	std::string url = req->getPath();
	Log("NOTE") << "URL:" << url;
	/**
 	 *  \note
 	 *  	禁用对已知不会成功的网站,
 	 *  	例如谷歌浏览器总是在后台对google发请求，
 	 *  	然而如果代理服务器在国内这是不会成功的，
 	 *  	然而这会增加代理服务器的负担，
 	 *  	所以干脆拉黑好了
 	 */  	
	if(in_black_list(url)) {					
		Log("NOTE") << "DROP " << url;
		res->setStatus(500);
		return;
	}
	if(req->getMethod() == "CONNECT") {				/**< ssl代理请求 */
		std::string host;
		std::string port = "https";							
		StringTokenizer st(url, ':');
		if(!st.hasMoreTokens()) {
			res->setStatus(400);
			return;
		}
		host = st.nextToken();
		if(st.hasMoreTokens())
			port = st.nextToken();
		TcpConnect(server_.service(), host, port, [=](ConnectionPtr conn) {
			if(!conn) {
				Log("ERROR") << "Connect to " << host << ":" << port << " failed";
				res->setStatus(500);
				return;
			}
			tunnel(res->connection(), conn);
		});
	} else {
		if(req->getQueryString() != "")
			url += "?" + req->getQueryString();
		client_.request(req->getMethod(), url, 
			[=](ResponsePtr response) {
				if(!response)
					return;
				res->setStatus(response->getStatus());
				res->setMessage(response->getMessage());
				res->setVersion(response->getVersion());
				if(response->getHeader("Transfer-Encoding") && 
					*response->getHeader("Transfer-Encoding") == "chunked")
					response->delHeader("Transfer-Encoding");
				res->delHeader("Connection");
				for(auto h : response->getHeaderMap())
					res->addHeader(h.name, h.value);
				res->addHeader("Proxy-Connection", "close");
				if(response->in().rdbuf()->in_avail()) {
					std::stringstream ss;
					ss << response->in().rdbuf();
					res->out() << ss.str();
				}
			},
			[=](RequestPtr request) {
				if(!request)
					return;
				req->delHeader("Proxy-Connection");
				for(auto h : req->getHeaderMap())
					request->addHeader(h.name, h.value);
				if(req->in().rdbuf()->in_avail()) {		/**< 判断是否有数据, [重要]*/
					std::stringstream ss;
					ss << req->in().rdbuf();
					request->out() << ss.str();
				}
			}
		);
	}
}

int 
main(int argc, char* argv[])
{
	try {
		std::stringstream config("{\"http port\":\"8888\"}");
		Server server(config, 1);						/**< 在8888端口监听 */
		server.addHandler("", new ProxyHandler(server));	/**< 监听所有路径上的报文 */
		server.run(1);							/**< 给io_service 10个线程 */
	} catch(std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
