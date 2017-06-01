#ifndef __SplitString__H_
#define __SplitString__H_

#include <iostream>
#include <vector>
#include <string>

namespace IBox {

class SplitString {
public:	
	SplitString(std::string str, std::string delimiters);
	void add(std::string str);
	void erase(std::string str);
	bool exist(std::string str);
	std::string toString();
private:
	std::string mDelimiters ;
	std::vector<std::string> mSubStrArray;
};

} // namespace IBox

#endif


