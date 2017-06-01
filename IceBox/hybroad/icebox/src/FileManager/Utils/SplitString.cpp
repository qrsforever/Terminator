#include "SplitString.h"

namespace IBox {
	
SplitString::SplitString(std::string str, std::string delimiters) {
	std::size_t found = str.find_first_of(delimiters);
	while (found != std::string::npos) {
		mSubStrArray.push_back(str.substr(0, found));
		str = str.substr(found + delimiters.size());
		found = str.find_first_of(delimiters);
	}
	if (str.size() > 0)
		mSubStrArray.push_back(str);
	mDelimiters = delimiters;
}

void SplitString::add(std::string str) {
	std::vector<std::string>::iterator it;
	for (it = mSubStrArray.begin(); it < mSubStrArray.end(); it++) {
		if (str.compare(*it) == 0)
			return ;
	}
	mSubStrArray.push_back(str);
}

void SplitString::erase(std::string str) {
	std::vector<std::string>::iterator it;
	for (it = mSubStrArray.begin(); it < mSubStrArray.end(); it++) {
		if (str.compare(*it) == 0) {
			mSubStrArray.erase(it);
			return ;
		}	
	}
}

bool SplitString::exist(std::string str) {
	std::vector<std::string>::iterator it;
	for (it = mSubStrArray.begin(); it < mSubStrArray.end(); it++) {
		if (str.compare(*it) == 0)
			return true;
	}
	return false;
}

std::string SplitString::toString() {
	std::string str = "";
	for (int i = 0; i < mSubStrArray.size(); i++) {
		str += mSubStrArray[i];
		if (i != mSubStrArray.size() - 1)
			str += mDelimiters;
	}
	return str;
}

} // namespace IBox

