#pragma once
#include <ctime>
#include <string>
#include <vector>
#include <cstdio>
#include "exception.hh"

struct BufferOverflow : Exception {
	using Exception::Exception;
};

struct TokenError : Exception {
	using Exception::Exception;
};

static inline std::string
localTime(time_t t)
{
	char lc_time[64] = { 0 };
	struct tm *lct = localtime(&t);
	if(!strftime(lc_time, sizeof(lc_time), "%a, %d-%h-%G %H:%M:%S", lct))
		DEBUG_THROW(BufferOverflow, "while get local time");
	return lc_time;
}

static inline std::string
gmtTime(time_t t)
{
	char gmt_time[64] = { 0 };
	struct tm *gmt = gmtime(&t);
	if(!strftime(gmt_time, sizeof(gmt_time), "%a, %d-%h-%G %H:%M:%S", gmt))
		DEBUG_THROW(BufferOverflow, "while get gmt time");
	return gmt_time;
}

template<typename _type, typename... _tParams>
constexpr auto
peek(_type&& arg, _tParams&&...)
{
        return std::forward<_type>(arg);
}


/**
 * \brief Split a container by a special item in it.
 * \param c The container to be splited.
 * \param i An item the container splited by.
 * \return A vector which stored sub containers after spliting.
 * \note
 *      The _tContainer should have operator+=, which append item
 *      to the end of that container.
 *      The _tContainer should provide a default constructor which
 *      construct a container which hase no items.
 *      The _tContainer should also can be contained by vector.
 *      The _tContainer should provide begin and end member functions.
 *      The _tContainer should have size member function, which return
 *      the number of items it contains.
 *      The _tItem should have operator== which compare two items.
 */ 
template<typename _tContainer, typename _tItem> 
std::vector<_tContainer> 
explode(_tContainer& c, const _tItem& i)
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

template<typename _tContainer, typename _tItem> 
std::vector<_tContainer> 
explode(_tContainer& c, const _tItem& item, const _tItem& lefts...)
{
	for(auto& it : c) {
		if(it == item)
			it = peek(lefts);
	}
	return explode(c, lefts);	
}

/**
 * \brief Split a string by given charactor.
 */
class StringTokenizer
{
public:
	StringTokenizer(std::string str, char delim1, char lefts...)
		:tokens(explode(str, delim1, lefts)) {}

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

time_t gmtToTime(const std::string& gmt_time);
