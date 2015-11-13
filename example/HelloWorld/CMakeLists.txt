ADD_EXECUTABLE(HelloWorld HelloWorld.cc)
INCLUDE_DIRECTORIES(../../inc)
TARGET_LINK_LIBRARIES(HelloWorld ahttpd)
