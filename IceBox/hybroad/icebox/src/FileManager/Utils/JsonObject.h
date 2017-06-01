#ifndef __JsonObject__H_
#define __JsonObject__H_

extern "C" { 
#include "json.h"
#include "json_tokener.h"
}

#include <string>

namespace IBox {

class JsonObject {

public:
	JsonObject();
	JsonObject(const char*);
	JsonObject(std::string);
	~JsonObject();
	std::string getString(std::string);
	int getInt(std::string);
	std::string toString();
	bool add(std::string, std::string);
	bool add(std::string, int);
	void del(std::string key);
	void clear();
	
protected:	
	struct json_object* mJsonObj;
};

bool isJsonString(const char*);

} /* namespace IBox */	

#endif

