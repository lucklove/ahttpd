#pragma once

#include <exception>
#include <ostream>

class Exception : public std::exception {
private:
	const char *const xm_pszFile;
	const unsigned long xm_ulLine;

	const char *const xm_pszMessage;
	const unsigned long xm_ulCode;

public:
	Exception(const char *pszFile, unsigned long ulLine,
		const char *pszMessage, unsigned long ulCode = 0) noexcept
		: xm_pszFile(pszFile), xm_ulLine(ulLine), xm_pszMessage(pszMessage), xm_ulCode(ulCode) {}

public:
	const char *what() const noexcept override {
		return xm_pszMessage;
	}

	const char *getFile() const noexcept {
		return xm_pszFile;
	}

	unsigned long getLine() const noexcept {
		return xm_ulLine;
	}

	const char *getMessage() const noexcept {
		return xm_pszMessage;
	}

	unsigned long getCode() const noexcept {
		return xm_ulCode;
	}
};

inline std::ostream& 
operator<<(std::ostream& os, const Exception& e)
{
	return os << "Exception in " << e.getFile() << " line: " 
		<< e.getLine() << ":" << e.what();
}

#define DEBUG_THROW(etype_, ...)			\
do {							\
	etype_ e_ (__FILE__, __LINE__, __VA_ARGS__);	\
	throw e_;					\
} while(false)

