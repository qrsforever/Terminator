#include "ExcludeFilesFilter.h"

#include <stdio.h>

namespace IBox {

bool 
ExcludeFilesFilter::check(std::string path) 
{
	std::vector<std::string>::iterator it;
	std::string name = path.substr(path.find_last_of("/") + 1);	
		
	if (name[0] == '.')		//ignore the hidden files and folder.
		return true;
	
	for (it = mExcludeFiles.begin(); it < mExcludeFiles.end(); it++) {
        std::size_t found = (*it).find("*");
        if (found != std::string::npos) {
            std::string substr = (*it).substr(found + 1);
            if (path.size() > substr.size()
            	&& path.compare(path.size() - substr.size(), substr.size(), substr) == 0)
                return true;
        } else {
            if (path.compare(*it) == 0)
                return true;
        }
    }
    
    return false;	
}

void 
ExcludeFilesFilter::add(std::string excludeFiles)
{
	std::vector<std::string>::iterator it;
		
	for (it = mExcludeFiles.begin(); it < mExcludeFiles.end(); it++) {
        if ((*it).compare(excludeFiles) == 0)
        	return ;
    }	
    
    mExcludeFiles.push_back(excludeFiles);
}

} // namespace IBox