#include "DateTime.h"

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

namespace IBox {

std::string DateTime::strftime(time_t t, std::string format)
{
	std::string timeStr;
    char buf[64] = {0};
    struct tm* ptm;
    ptm = localtime(&t);
    
	if (format.compare("YYYYMMDDHHMMSS") == 0) {
		snprintf(buf, sizeof(buf), "%04d%02d%02d%02d%02d%02d", 
			ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	} else if (format.compare("YYYY-MM-DD HH:MM:SS") == 0) {
		snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", 
			ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	}	
	
	timeStr = buf;
    return timeStr;
}

std::string DateTime::now()
{
	return strftime(time(NULL), "YYYY-MM-DD HH:MM:SS");
}
	
} // namespace IBox