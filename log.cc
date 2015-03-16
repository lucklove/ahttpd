#include "log.hh"
#include "exception.hh"
#include <ctime>

struct ExpectMoreBuffer : Exception {
	using Exception::Exception;
};

std::string
Log::local_time()
{
        char lc_time[64] = { 0 };
        time_t t = time(nullptr);
    
        struct tm *lct = localtime(&t);
        if(!strftime(lc_time, sizeof(lc_time), "%a, %d-%h-%G %H:%M:%S", lct))
                DEBUG_THROW(ExpectMoreBuffer, "while get local time");
        return lc_time;
}   
