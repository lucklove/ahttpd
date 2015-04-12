#pragma once

#include <vector>

template<typename _tContainer, typename _tItem> 
const std::vector<_tContainer> 
explode(const _tContainer& c, const _tItem& i)
{
        _tContainer buff;
        std::vector<_tContainer> v;
    
        for(auto n:c)
        {
                if(n != i) {
                        buff += n;  
                } else if(n == i && buff.size() != 0) { 
                                v.push_back(buff); 
                                buff = _tContainer(); 
                }
        }
        if(buff.size() != 0)  
                v.push_back(buff);
    
        return v;
}

class StringTokenizer
{
public:
        /**
         * \param str The string to be splited.
         * \param delim Dest string was splited by this charactor.
         */ 
        StringTokenizer(std::string str, char delim = ' ')
                :tokens(explode(str, delim)) {}
    
        StringTokenizer() = default;
        StringTokenizer(const StringTokenizer &) = default;
    
        /**
         * \brief Check if there are more tokens in the tokenizer.
         * \return If have, true, else, false.
         */ 
        bool hasMoreTokens() { return !tokens.empty(); }

        /**
         * \brief Get next token in the tokenizer.
         * \return Next token.
         */ 
        std::string nextToken()
        {
                if(tokens.empty())
                        DEBUG_THROW(TokenError, "No more tokens");
                std::string ret = tokens.front();
                tokens.erase(tokens.begin());
                return ret;
        }

        std::vector<std::string>::iterator begin() { return tokens.begin(); };
        std::vector<std::string>::iterator end() { return tokens.end(); };
        size_t size() { return tokens.size(); }
private:
	std::vector<std::string> tokens;
};
