SET(TEST_SOURCES
	test.cc
	request/request.cc
	connection/connection.cc
	client/client.cc
	client/client_check_cookie.cc
	mime/mime.cc
	base64/base64.cc
	cookie/cookie.cc
	utils/time.cc
	utils/string_tokenizer.cc
	utils/url.cc
	utils/ip.cc
	utils/path.cc
	utils/domain.cc
	ThreadPool/ThreadPool.cc
	server/server.cc
	exception/exception.cc
	parser/parser.cc
	net/net.cc
	fcgi/fcgi.cc
#	mail/mail.cc
)
ADD_EXECUTABLE(ahttpd_test ${TEST_SOURCES})
INCLUDE_DIRECTORIES(../src)
TARGET_LINK_LIBRARIES(ahttpd_test ahttpd boost_system ssl crypto pthread tcmalloc)
ADD_CUSTOM_TARGET(run_test COMMAND ${CMAKE_BINARY_DIR}/test/ahttpd_test DEPENDS ahttpd_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
