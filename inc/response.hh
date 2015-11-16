#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "header.hh"
#include "package.hh"
#include "ptrs.hh"

namespace ahttpd {

class Response : public Package, 
    public std::enable_shared_from_this<Response> {
public:
    typedef unsigned short status_t;
    enum {
        Continue = 100, 
        Switching = 101, 
        Processing = 102,
        Ok = 200, 
        Created = 201, 
        Accepted = 202, 
        Non_Authoritative_Information = 203, 
        No_Content = 204, 
        Reset_Content = 205, 
        Partial_Content = 206, 
        Muti_Status = 207,
        Multiple_Choices = 300, 
        Moved_Permanently = 301, 
        Moved_Temporarily = 302, 
        See_Other = 303, 
        Not_Modified = 304, 
        Use_Proxy = 305,
        Temporary_Redirect = 307,
        Bad_Request = 400, 
        Unauthorized = 401, 
        Payment_Required = 402, 
        Forbidden = 403, 
        Not_Found = 404,
        Method_Not_Allowed = 405,
        Not_Acceptable = 406,
        Proxy_Authentication_Required = 407,
        Request_Time_Out = 408,
        Conflict = 409,
        Gone = 410,
        Length_Required = 411,
        Precondition_Failed = 412,
        Request_Entity_Too_Large = 413,
        Request_Uri_Too_Large = 414,
        Unsupported_Media_Type = 415,
        Requested_Range_Not_Satisfiable = 416,
        Expectation_Failed = 417,
        Internal_Server_Error = 500, 
        Not_Implemented = 501, 
        Bad_Gateway = 502, 
        Service_Unavailable = 503,
        Gateway_Time_Out = 504,
        HTTP_Version_Not_Supported = 505
    };

    Response(ConnectionPtr connection) :
        Package(connection), status_(Ok) {}
    ~Response() override;

    void setMimeType(const std::string& mime) { 
        delHeader("Content-Type");
        addHeader("Content-Type", mime);
    }
    std::string getVersion() { return version_; }
    void setVersion(const std::string& version) { version_ = version; }
    status_t getStatus() { return status_; }
    void setStatus(status_t status) { status_ = status; }
    std::string getMessage() { return msg_; }
    void setMessage(const std::string& msg) { msg_ = msg; }
    void flush();

    void setCookie(const response_cookie_t& cookie) {
        std::string header_val = cookie.key;
        if(cookie.val != "")
            header_val += "=" + cookie.val;
        if(cookie.expires)
            header_val += "; expires=" + gmtTime(cookie.expires) + " GMT";
        if(cookie.domain != "")
            header_val += "; domain=" +  cookie.domain;
        if(cookie.path != "")
            header_val += "; path=" + cookie.path;
        if(cookie.secure)
            header_val += "; secure";
        if(cookie.httponly)
            header_val += "; HttpOnly";
        addHeader("Set-Cookie", header_val);    
    }

    const std::vector<response_cookie_t>& cookieJar() {
        return cookie_jar_;
    }
    
private:
    std::string version_;
    status_t status_;
    std::string msg_;
    std::vector<response_cookie_t> cookie_jar_;
    void parseCookie() {
        cookie_jar_ = parseResponseCookie(getHeaders("Set-Cookie"));        
    }
    friend void parseResponse(ConnectionPtr, std::function<void(ResponsePtr)>);
};

}    /**< namespace ahttpd */
