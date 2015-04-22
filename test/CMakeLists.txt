SET(TEST_SOURCES
	test.cc
	mime/mime.cc
	base64/base64.cc
)
ADD_EXECUTABLE(test ${TEST_SOURCES})
INCLUDE_DIRECTORIES(../src)
TARGET_LINK_LIBRARIES(test shttpd boost_system ssl crypto pthread)
