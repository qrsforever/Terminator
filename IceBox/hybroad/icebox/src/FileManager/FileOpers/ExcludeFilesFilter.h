#ifndef __ExcludeFilesFilter__H_
#define __ExcludeFilesFilter__H_

#include "FileNameFilter.h"

#include <string>
#include <vector>

namespace IBox {

class ExcludeFilesFilter : public FileNameFilter {
public:	
	bool check(std::string name);
	void add(std::string excludeFiles);
private:
	std::vector<std::string> mExcludeFiles;		
};


} // namespace IBox

#endif
