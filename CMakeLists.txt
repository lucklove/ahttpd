project(httpd)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_COMPILER g++)
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

add_library(httpd ${HTTPD_SRCS})

target_link_libraries(httpd ssl crypto pthread)
