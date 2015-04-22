ADD_EXECUTABLE(client client.cc)
INCLUDE_DIRECTORIES(../../src)
TARGET_LINK_LIBRARIES(client shttpd)
