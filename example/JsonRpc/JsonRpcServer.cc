#include "JsonRpcServer.hh"
#include "server.hh"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::write_json;
using boost::property_tree::ptree;
using boost::property_tree::read_json;

bool
JsonRpcServer::regist(const std::string& user, const std::string& pass)
{
	for(auto u : users_) {
		if(u.username == user)
			return false;
	}
	users_.push_back({ user, pass });
	return true;
}

bool
JsonRpcServer::add(const std::string& host, const std::string& guest, const std::string& start_time,
	const std::string& end_time, const std::string& title)
{
	for(auto m : meetings_) {
		if(m.host == host || m.guest == host || m.host == guest || m.guest == guest) {
			if((m.start_time <= start_time && m.end_time > start_time) ||
				(m.start_time < end_time && m.end_time >= end_time))
				return false;
		}
	}
	meetings_.push_back({ host, guest, start_time, end_time, title, id_index_++ });
	return true;
}

void
JsonRpcServer::query(const std::string& username, const std::string& start_time, 
	const std::string& end_time, ResponsePtr res, int id)
{
	for(auto m : meetings_) {
		if(m.host == username || m.guest == username) {
			res->out() << "{\"result\": {\"host\": \"" << m.host << "\", \"guest\": \""
				<< m.guest << "\", \"start time\": \"" << m.start_time << "\", \"end time\": \""
				<< m.end_time << "\", \"title\": \"" << m.title << "\", \"meetingid\": \""
				<< m.meetingid << "\"}, \"error\": null, \"id\": " << id << "}";
		}
	}	
}

bool
JsonRpcServer::del(const std::string& username, int meetingid)
{
	for(auto it = meetings_.begin(); it != meetings_.end(); ++it) {
		if(it->meetingid == meetingid) {
			if(it->host == username) {
				meetings_.erase(it);
				return true;
			} else {
				return false;
			}
		}
	}	
	return false;
}

void
JsonRpcServer::clear(const std::string& username)
{
	for(auto it = meetings_.begin(); it != meetings_.end(); ++it) {
		if(it->host == username)
			meetings_.erase(it);
	}	
}

bool
JsonRpcServer::check(const std::string& user, const std::string& pass)
{
	for(auto u : users_) {
		if(u.username == user && u.password == pass)
			return true;
	}
	return false;
}
	
void 
JsonRpcServer::handleRequest(RequestPtr req, ResponsePtr rep) {
	int id = 0;
	try {
		ptree pt;
		read_json<ptree>(req->in(), pt);
		id = pt.get<int>("id");
		std::string func_name = pt.get<std::string>("method");
		ptree params = pt.get_child("params");
		std::string username = params.get<std::string>("username");
		std::string password = params.get<std::string>("password");
		if(func_name != "register" && !check(username, password)) {
			rep->out() << "{\"result\": null, \"error\": \"forbiden\", \"id\": " << id << "}";
			return;
		}
		if(func_name == "register") {
			if(regist(username, password)) {
				rep->out() << "{\"result\": \"success\", \"error\": null, \"id\": " << id << "}";
			} else {
				rep->out() << "{\"result\": null, \"error\": \"exist\", \"id\": " << id << "}";
			}
			return;
		} else if(func_name == "add") {
			std::string guest = params.get<std::string>("guest");
			std::string start_time = params.get<std::string>("start");
			std::string end_time = params.get<std::string>("end");
			std::string title = params.get<std::string>("title");
			if(add(username, guest, start_time, end_time, title)) {
				rep->out() << "{\"result\": \"success\", \"error\": null, \"id\": " << id << "}";
			} else {
				rep->out() << "{\"result\": null, \"error\": \"conflict\", \"id\": " << id << "}";
			}
			return;
		} else if(func_name == "query") {
			std::string start_time = params.get<std::string>("start");
			std::string end_time = params.get<std::string>("end");
			query(username, start_time, end_time, rep, id);
		} else if(func_name == "delete") {
			int meetingid = params.get<int>("meetingid");
			if(del(username, meetingid)) {
				rep->out() << "{\"result\": \"success\", \"error\": null, \"id\": " << id << "}";
			} else {
				rep->out() << "{\"result\": null, \"error\": \"failed\", \"id\": " << id << "}";
			}
		} else if(func_name == "clear") {
			clear(username);
			rep->out() << "{\"result\": \"success\", \"error\": null, \"id\": " << id << "}";
		}	
		rep->out() << "{\"result\": null, \"error\": "
			"\"function not implement\", \"id\": " 
			<< pt.get<int>("id") << "}";
	} catch(std::exception& e) {
		rep->out() << "{\"result\": null, \"error\": \"" << e.what() << "\", \"id\": " << id << "}";
	}
}

int 
main(int argc, char* argv[])
{
	try {
		asio::io_service io_service;
		Server server(io_service, "8888");
		server.addHandler("/jsonrpc", new JsonRpcServer(&server));
		server.run(10);	
	} catch(std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
