project(httpd)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_COMPILER g++)
set(HTTPD_SRCS SHARED
	src/package.cc 
	src/TcpConnection.cc 
	src/request.cc
	src/response.cc 
	src/parser.cc 
	src/ThreadPool.cc 
	src/server.cc 
	src/log.cc 
	src/RequestHandler.cc 
	src/SslConnection.cc 
)
set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++1y -Wall")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(httpd ${HTTPD_SRCS})

target_link_libraries(httpd ssl crypto pthread)

install(TARGETS httpd
	LIBRARY DESTINATION lib
)

install(FILES
	src/header.hh
	src/package.hh
	src/ptrs.hh
	src/RequestHandler.hh
	src/request.hh
	src/response.hh
	src/server.hh
	src/ThreadPool.hh
	DESTINATION include/shttpd
)
