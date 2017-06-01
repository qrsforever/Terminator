#include "JsonObject.h"

#include <string.h>
#include <string>
#include <iostream>
#include <stdio.h>

namespace IBox {

JsonObject::JsonObject() {
    mJsonObj = json_object_new_object();
}

JsonObject::JsonObject(const char* s) {
    mJsonObj = json_tokener_parse(s);
}

JsonObject::JsonObject(std::string s) {
    mJsonObj = json_tokener_parse(s.c_str());
}

JsonObject::~JsonObject() {
    json_object_put(mJsonObj);
}

std::string JsonObject::getString(std::string key) {
	std::string str("");
	
    struct json_object* obj = json_object_object_get(mJsonObj, key.c_str());
    if (obj)
        str = json_object_get_string(obj);
	
    return str;
}

int JsonObject::getInt(std::string key) {
	int value = 0;
	
    struct json_object* obj = json_object_object_get(mJsonObj, key.c_str());
    if (obj) 
        value = json_object_get_int(obj);
    
    return value;
}

std::string JsonObject::toString() {
    return json_object_to_json_string(mJsonObj);
}

bool JsonObject::add(std::string key, std::string value) {
    struct json_object* obj = json_object_new_string(value.c_str());
    if (obj) {
        json_object_object_add(mJsonObj, key.c_str(), obj);
        return true;
    }
    return false;
}

bool JsonObject::add(std::string key, int value) {
    struct json_object* obj = json_object_new_int(value);
    if (obj) {
        json_object_object_add(mJsonObj, key.c_str(), obj);
        return true;
    }
    return false;
}

void JsonObject::del(std::string key) {
	json_object_object_del(mJsonObj, key.c_str());
}

void JsonObject::clear() {
    json_object_put(mJsonObj);
    mJsonObj = json_object_new_object();
}


bool isJsonString(const char* str) {
    if (str && str[0] == '{' && str[strlen(str) - 1] == '}')
        return true;
    return false;
}

} /* namespace IBox */


