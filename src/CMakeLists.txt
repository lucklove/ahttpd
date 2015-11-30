SET(AHTTPD_SRCS SHARED
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
    TcpConnection.cc
    exception.cc
)

INCLUDE_DIRECTORIES(../inc)
ADD_LIBRARY(ahttpd ${AHTTPD_SRCS})
TARGET_LINK_LIBRARIES(ahttpd ssl crypto pthread boost_system)

INSTALL(TARGETS ahttpd LIBRARY DESTINATION lib)
INSTALL(FILES
    ../inc/ahttpd.hh
    ../inc/log.hh
    ../inc/header.hh
    ../inc/package.hh
    ../inc/ptrs.hh
    ../inc/RequestHandler.hh
    ../inc/request.hh
    ../inc/response.hh
    ../inc/server.hh
    ../inc/client.hh
    ../inc/MimeType.hh
    ../inc/ThreadPool.hh
    ../inc/base64.hh
    ../inc/exception.hh
    ../inc/mail.hh
    ../inc/cookie.hh
    ../inc/utils.hh
    ../inc/net.hh
    ../inc/fcgi.hh
    DESTINATION include/ahttpd
)

