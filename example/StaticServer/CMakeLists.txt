ADD_EXECUTABLE(StaticServer StaticServer.cc)
INCLUDE_DIRECTORIES(../../src)
TARGET_LINK_LIBRARIES(StaticServer shttpd)
