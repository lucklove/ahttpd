#include "client.hh"
#include "response.hh"
#include "request.hh"
#include "utils.hh"
#include <string>

namespace {

size_t id_index_ = 0;

void
main_loop()
{
	std::sting line;
	std::cout << ">>";
	std::cout.flush();
	getline(std::cin, line);
	StringTokenizer st(line);
	std::string method = st.nextToken();
	std::string mesg = "{\"method\": \"" << method << "\", \"params\": ";
	std::string params{};
	if(method == "register") {
		params = "{\"username\": \"" << st.nextToken() << "\", \"password\": \""
			<< st.nextToken() << "\"}";
	} else if(method == "add") {
		params = "{\"username\": \"" << st.nextToken() << "\", \"password\": \""
			<< st.nextToken() << "\", \"guest\": \"" << st.nextToken() << "\", \"start\": \""
			<< st.nextToken() << "\", \"end\": \"" << st.nextToken() << "\", \"title\": \""
			<< st.nextToken() << "\"}";
	} else if(method == "query") {
		params = "{\"username\": \"" << st.nextToken() << "\", \"password\": \""
			<< st.nextToken() << "\", \"start\": \"" << st.nextToken() << "\", \"end\": \""
			<< st.nextToken() << "\"}";
	} else if(method == "delete") {
		params = "{\"username\": \"" << st.nextToken() << "\", \"password\": \""
			<< st.nextToken() << "\", \"meetingid\": " << st.nextToken() << "}";
	} else if(method == "clear") {
		params = "{\"username\": \"" << st.nextToken() << "\", \"password\": \""
			<< st.nextToken() << "\"}";
	}
}

}
int
main(int argc, char *argv[])
{
	
