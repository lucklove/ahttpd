project(shttpd)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_COMPILER clang++)
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
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++1y -Wall -g")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(shttpd ${HTTPD_SRCS})

target_link_libraries(shttpd ssl crypto pthread)

install(TARGETS shttpd
	LIBRARY DESTINATION lib
)

install(FILES
	src/log.hh
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
