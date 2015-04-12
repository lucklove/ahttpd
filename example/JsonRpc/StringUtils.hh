/**************************************************************************
 * stringtokenizer.h                                                      * 
 * Copyright (C) 2014 Joshua <gnu.crazier@gmail.com>                      *
 **************************************************************************/

#pragma once

#include "explode.hh"
#include "exception.hh"
#include <string>
#include <vector>
#include <regex>

namespace zbase {

struct TokenError : public Exception {
	using Exception::Exception;
};

struct SerializeAble {
	virtual std::string serialize() = 0;
	virtual ~SerializeAble() {};
};

/**
 * \brief Split a string by given charactor.
 */ 
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

/**
 * \brief Collect all words in a string.
 */
class WordCollector {
public:
/**
 * \brief Constructor.
 * \param line Dest string to be collect words.
 */ 
	WordCollector(std::string line) 
	{
		std::regex r("\\b\\w+\\b");				/**< r must live in for loop. */
		for(std::sregex_iterator it(line.begin(), line.end(), r), it_end; it != it_end; ++it)
			word_list.push_back(it->str());
	}
	auto begin() { return word_list.begin(); }                      /**< since c++1y */
	auto end() { return word_list.end(); }                          /**< since c++1y */
	auto size() { return word_list.size(); }			/**< since c++1y */
private:
	std::vector<std::string> word_list;
};

bool
startsWith(std::string s, std::string seq)
{
	std::regex r("^" + s);
	std::smatch m;
	return std::regex_search(seq, m, r);
}

}		/**< namespace zbase */
