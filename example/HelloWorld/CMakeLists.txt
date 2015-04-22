ADD_EXECUTABLE(HelloWorld HelloWorld.cc)
INCLUDE_DIRECTORIES(../../src)
TARGET_LINK_LIBRARIES(HelloWorld shttpd)
