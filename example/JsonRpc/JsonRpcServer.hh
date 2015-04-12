#pragma once

#include "server.hh"
#include <vector>

struct User {
	std::string username;
	std::string password;
};

struct Meeting {
	std::string host;
	std::string guest;
	std::string start_time;
	std::string end_time;
	std::string title;
	int meetingid;
};

struct JsonRpcServer : RequestHandler {
public:
	using RequestHandler::RequestHandler;
	void handleRequest(RequestPtr req, ResponsePtr res);
private:
	bool regist(const std::string& username, const std::string& password);
	bool add(const std::string& host, const std::string& guest, 
		const std::string& start, const std::string& end, const std::string& title);
	void query(const std::string& username, const std::string& start, const std::string& end, ResponsePtr res, int id);
	bool del(const std::string& username, int meetingid);
	void clear(const std::string& username);
	bool check(const std::string& username, const std::string& password);
	int id_index_ = 0;
	std::vector<User> users_;
	std::vector<Meeting> meetings_;
};
	
