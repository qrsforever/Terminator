#ifndef __DateTime__H_
#define __DateTime__H_

#include <stdint.h>
#include <string>

namespace IBox {

class DateTime {
public:
    static std::string strftime(time_t t, std::string format);
	static std::string now();
};

} // namespace IBox

#endif // _DateTime_H_
