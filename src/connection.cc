#include "connection.hh"

#define ASYNC_APPLY(op, func, ...)								\
enqueue##op([=, ptr = shared_from_this()] {							\
	socket_t sock = socket();								\
	auto handle = [this, handler, ptr](const boost::system::error_code& e, size_t n) {	\
		handler(e, n);									\
		dequeue##op();									\
	};											\
	if(sock.type == socket_type::ordinary) {						\
		func(*sock.ordinary_socket, __VA_ARGS__, handle);				\
	} else if(sock.type == socket_type::ssl) {						\
		func(*sock.ssl_socket, __VA_ARGS__, handle);					\
	} else {										\
		Log("PANIC") << "UNKNOWN SOCKET TYPE";						\
		assert(false && "unknown socket type");						\
	}											\
})

void 
Connection::async_read(std::function<size_t(const boost::system::error_code &, size_t)> completion,
	std::function<void(const boost::system::error_code &, size_t)> handler) 
{
	ASYNC_APPLY(Read, boost::asio::async_read, readBuffer(), completion);
}

void 
Connection::async_read_until(const std::string& delim, 
	std::function<void(const boost::system::error_code &, size_t)> handler)
{
	ASYNC_APPLY(Read, boost::asio::async_read_until, readBuffer(), delim);
}
	
void 
Connection::async_write(const std::string& msg, 
	std::function<void(const boost::system::error_code&, size_t)> handler)
{
	ASYNC_APPLY(Write, boost::asio::async_write, boost::asio::buffer(msg));
}
