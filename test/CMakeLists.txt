SET(TEST_SOURCES
	test.cc
	connection/connection.cc
	client/client.cc
	mime/mime.cc
	base64/base64.cc
	cookie/cookie.cc
	utils/time.cc
	utils/string_tokenizer.cc
	utils/params.cc
	ThreadPool/ThreadPool.cc
	request/request.cc
	response/response.cc
	server/server.cc
	exception/exception.cc
	mail/mail.cc
)
ADD_EXECUTABLE(ahttpd_test ${TEST_SOURCES})
INCLUDE_DIRECTORIES(../src)
TARGET_LINK_LIBRARIES(ahttpd_test ahttpd boost_system ssl crypto pthread)
ADD_CUSTOM_TARGET(run_test COMMAND ${CMAKE_BINARY_DIR}/test/ahttpd_test DEPENDS ahttpd_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
