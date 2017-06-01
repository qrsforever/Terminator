#ifndef __FileNameFilter__H_
#define __FileNameFilter__H_

#include <string>

namespace IBox {

class FileNameFilter {
public:	
	virtual bool check(std::string name) = 0;
};

} // namespace IBox

#endif
