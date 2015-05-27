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
	net.cc
	fcgi.cc
)
ADD_LIBRARY(ahttpd ${SHTTPD_SRCS})
TARGET_LINK_LIBRARIES(ahttpd ssl crypto pthread boost_system)
INSTALL(TARGETS ahttpd LIBRARY DESTINATION lib)
INSTALL(FILES
	ahttpd.hh
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
	net.hh
	DESTINATION include/ahttpd
)
