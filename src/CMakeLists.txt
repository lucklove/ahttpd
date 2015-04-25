SET(SHTTPD_SRCS SHARED
	package.cc
	connection.cc 
	response.cc 
	request.cc 
	parser.cc 
	ThreadPool.cc 
	server.cc 
	RequestHandler.cc 
	SslConnection.cc 
	client.cc
	MimeType.cc
	base64.cc
	mail.cc
	utils.cc
)
ADD_LIBRARY(shttpd ${SHTTPD_SRCS})
TARGET_LINK_LIBRARIES(shttpd ssl crypto pthread boost_system)
INSTALL(TARGETS shttpd LIBRARY DESTINATION lib)
INSTALL(FILES
	log.hh
	header.hh
	package.hh
	ptrs.hh
	RequestHandler.hh
	request.hh
	response.hh
	server.hh
	client.hh
	MimeType.hh
	ThreadPool.hh
	base64.hh
	exception.hh
	mail.hh
	cookie.hh
	utils.hh
	DESTINATION include/shttpd
)
