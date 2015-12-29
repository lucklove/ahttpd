#pragma once

#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "utils.hh"
#include "log.hh"

namespace ahttpd 
{

/**
 *\brief 存放request中的cookie
 *\example
 *  request_cookie_t cookie;
 *  cookie.setKey("key").setVal("val");
 */
struct request_cookie_t 
{
    std::string key;
    std::string val;
    request_cookie_t& setKey(const std::string& k) 
    {
        key = k;
        return *this;
    }
    request_cookie_t& setVal(const std::string& v) 
    {
        val = v;
        return *this;
    }
};

/**
 * \brief 存放response中的cookie
 * \example
 *      response_cookie_t cookie;
 *      cookie.setKey("key").setVal("val").setExpires(time() + 60);
 */
struct response_cookie_t 
{
    std::string key;
    std::string val;
    time_t expires = 0;
    std::string domain;
    std::string path;
    bool secure = false;
    bool httponly = false;
    response_cookie_t& setKey(const std::string& k) 
    {
        key = k;
        return *this;
    }
    response_cookie_t& setVal(const std::string& v) 
    {
        val = v;
        return *this;
    }
    response_cookie_t& setExpires(time_t e) 
    {
        expires = e;
        return *this;
    }
    response_cookie_t& setMaxAge(long age) 
    {
        expires = time(nullptr) + age;
        return *this;
    }
    response_cookie_t& setDomain(const std::string& d) 
    {
        domain = d;
        return *this;
    }
    response_cookie_t& setPath(const std::string& p) 
    {
        path = p;
        return *this;
    }
    response_cookie_t& setSecure() 
    {
        secure = true;
        return *this;
    }
    response_cookie_t& setHttpOnly() 
    {
        httponly = true;
        return *this;
    }
};

inline std::vector<request_cookie_t>
parseRequestCookie(const std::string& cookie_header)
{
    std::vector<request_cookie_t> cookie_jar;
    request_cookie_t cookie;
    StringTokenizer token_st(cookie_header, ' ', ';');
    while(token_st.hasMoreTokens()) 
    {
        StringTokenizer key_val_st(token_st.nextToken(), '=');
        if(key_val_st.hasMoreTokens())
            cookie.key = key_val_st.nextToken();        
        if(key_val_st.hasMoreTokens())
            cookie.val = key_val_st.nextToken();
        cookie_jar.push_back(cookie);
    }
    return cookie_jar;
}

inline response_cookie_t
parseResponseCookie(const std::string& cookie_header)
{
    response_cookie_t cookie = {};
    StringTokenizer st(cookie_header, ';');
    if(st.hasMoreTokens()) 
    {
        StringTokenizer key_val_st(st.nextToken(), '=');
        if(key_val_st.hasMoreTokens())
            cookie.key = key_val_st.nextToken();
        if(key_val_st.hasMoreTokens())
            cookie.val = key_val_st.nextToken();
    }
    while(st.hasMoreTokens()) 
    {
        StringTokenizer key_val_st(st.nextToken(), '=');
        std::string key;
        std::string val;
        if(key_val_st.hasMoreTokens())
            key = key_val_st.nextToken();
        int nonblank = key.find_first_not_of(" ");            /**< 去除开始的空白 */
        if(nonblank == static_cast<int>(key.npos))
            continue;
        key = key.substr(nonblank, key.size());
        if(key_val_st.hasMoreTokens())
            val = key_val_st.nextToken();
        if(strcasecmp(key.c_str(), "expires") == 0) 
        {
            if(!cookie.expires)
                cookie.expires = gmtToTime(val);
        } 
        else if(strcasecmp(key.c_str(), "max-age") == 0) 
        {
            try 
            {
                cookie.expires = time(nullptr) + boost::lexical_cast<long>(val);
            } 
            catch(boost::bad_lexical_cast &e) 
            {
                Log("ERROR") << e.what();
                cookie.expires = 0;
            }
        } 
        else if(strcasecmp(key.c_str(), "domain") == 0) 
        {
            cookie.domain = val;
        } 
        else if(strcasecmp(key.c_str(), "path") == 0) 
        {
            cookie.path = val;
        } 
        else if(strcasecmp(key.c_str(), "secure") == 0) 
        {
            cookie.secure = true;
        } 
        else if(strcasecmp(key.c_str(), "HttpOnly") == 0) 
        {
            cookie.httponly = true;
        }
    }
    return cookie;    
}

inline std::vector<response_cookie_t>
parseResponseCookie(const std::vector<std::string>& cookie_headers)
{
    std::vector<response_cookie_t> cookie_jar;
    for(const std::string& h : cookie_headers)
        cookie_jar.push_back(parseResponseCookie(h));
    return cookie_jar;
}

}    /**< namespace ahttpd */
