#pragma once

#include <map>
#include "utils.hh"

static inline std::map<std::string, std::string>
parseParams(const std::string& param_list)
{
	std::map<std::string, std::string> map;
	StringTokenizer st(param_list, '&');
	while(st.hasMoreTokens()) {
		StringTokenizer key_val_st(st.nextToken(), '=');
		if(!key_val_st.hasMoreTokens())
			continue;
		std::string key = key_val_st.nextToken();
		std::string val;
		if(key_val_st.hasMoreTokens())
			val = key_val_st.nextToken();
		map[key] = val;			
	}
	return map;
}
