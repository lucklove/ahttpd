#pragma once

#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "utils.hh"
#include "log.hh"

struct request_cookie_t {
	std::string key;
	std::string val;
};

struct response_cookie_t {
	std::string key;
	std::string val;
	time_t expires = 0;
	std::string domain;
	std::string path;
	bool secure = false;
	bool httponly = false;
};

static inline std::vector<request_cookie_t>
parseRequestCookie(const std::string& cookie_header)
{
	std::vector<request_cookie_t> cookie_jar;
	request_cookie_t cookie;
	StringTokenizer token_st(cookie_header, ' ', ';');
	while(token_st.hasMoreTokens()) {
		StringTokenizer key_val_st(token_st.nextToken(), '=');
		if(key_val_st.hasMoreTokens())
			cookie.key = key_val_st.nextToken();		
		if(key_val_st.hasMoreTokens())
			cookie.val = key_val_st.nextToken();
		cookie_jar.push_back(cookie);
	}
	return cookie_jar;
}

static inline response_cookie_t
parseResponseCookie(const std::string& cookie_header)
{
	response_cookie_t cookie;
	StringTokenizer st(cookie_header, ';');
	if(st.hasMoreTokens()) {
		StringTokenizer key_val_st(st.nextToken(), '=');
		if(key_val_st.hasMoreTokens())
			cookie.key = key_val_st.nextToken();
		if(key_val_st.hasMoreTokens())
			cookie.val = key_val_st.nextToken();
	}
	while(st.hasMoreTokens()) {
		StringTokenizer key_val_st(st.nextToken(), '=');
		std::string key;
		std::string val;
		if(key_val_st.hasMoreTokens())
			key = key_val_st.nextToken();
		int nonblank = key.find_first_not_of(" ");			/**< 去除开始的空白 */
		if(nonblank == static_cast<int>(key.npos))
			continue;
		key = key.substr(nonblank, key.size());
		if(key_val_st.hasMoreTokens())
			val = key_val_st.nextToken();
		if(strcasecmp(key.c_str(), "expires") == 0) {
			if(!cookie.expires)
				cookie.expires = gmtToTime(val);
		} else if(strcasecmp(key.c_str(), "max-age") == 0) {
			try {
				cookie.expires = boost::lexical_cast<time_t>(val);
			} catch(boost::bad_lexical_cast &e) {
				Log("ERROR") << e.what();
				cookie.expires = 0;
			}
		} else if(strcasecmp(key.c_str(), "domain") == 0) {
			cookie.domain = val;
		} else if(strcasecmp(key.c_str(), "path") == 0) {
			cookie.path = val;
		} else if(strcasecmp(key.c_str(), "secure") == 0) {
			cookie.secure = true;
		} else if(strcasecmp(key.c_str(), "HttpOnly") == 0) {
			cookie.httponly = true;
		}
	}
	return cookie;	
}

static inline std::vector<response_cookie_t>
parseResponseCookie(const std::vector<std::string>& cookie_headers)
{
	std::vector<response_cookie_t> cookie_jar;
	for(const std::string& h : cookie_headers)
		cookie_jar.push_back(parseResponseCookie(h));
	return cookie_jar;
}
