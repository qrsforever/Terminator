#ifndef __FileFilter__H_
#define __FileFilter__H_

#include <string>

namespace IBox {

class FileFilter {
public:	
	virtual void accept(std::string path) = 0;
};


} // namespace IBox

#endif
