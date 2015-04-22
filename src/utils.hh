#pragma once
#include <ctime>
#include <string>
#include "exception.hh"

struct BufferOverflow : Exception {
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
