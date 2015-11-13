ADD_EXECUTABLE(client client.cc)
INCLUDE_DIRECTORIES(../../inc)
TARGET_LINK_LIBRARIES(client ahttpd)
