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
	mail/mail.cc
    AsyncWrapper/AsyncWrapper.cc
)

SET(FCGI_TEST_SOURCES
    test.cc
    fcgi/fcgi.cc
)

SET(HTTPS_TEST_SOURCES
    test.cc
    server/https.cc
)

INCLUDE_DIRECTORIES(../inc .)

ADD_EXECUTABLE(ahttpd_test ${TEST_SOURCES})
TARGET_LINK_LIBRARIES(ahttpd_test ahttpd)
ADD_CUSTOM_TARGET(run_test COMMAND ${CMAKE_BINARY_DIR}/test/ahttpd_test DEPENDS ahttpd_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

ADD_EXECUTABLE(fcgi_test ${FCGI_TEST_SOURCES})
TARGET_LINK_LIBRARIES(fcgi_test ahttpd)
ADD_CUSTOM_TARGET(run_fcgi_test COMMAND ${CMAKE_BINARY_DIR}/test/fcgi_test DEPENDS fcgi_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

ADD_EXECUTABLE(https_test ${HTTPS_TEST_SOURCES})
TARGET_LINK_LIBRARIES(https_test ahttpd)
ADD_CUSTOM_TARGET(run_https_test COMMAND ${CMAKE_BINARY_DIR}/test/https_test DEPENDS https_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
