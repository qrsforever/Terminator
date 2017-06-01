#ifndef __JsonObjectArray__H_
#define __JsonObjectArray__H_

#include "JsonObject.h"

#include <string>

namespace IBox {

class JsonObjectArray : public JsonObject {
public:
	JsonObjectArray();
    JsonObjectArray(const char* s);
    JsonObjectArray(std::string s);
	~JsonObjectArray();
	int size();
	int add(std::string val);
	int put(int idx, std::string val);
	JsonObject get(int idx);
};

} /* namespace IBox */	

#endif
