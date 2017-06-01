#include "JsonObjectArray.h"

namespace IBox {
	
JsonObjectArray::JsonObjectArray() {
    mJsonObj = json_object_new_array();
}

JsonObjectArray::JsonObjectArray(const char* s) {
    mJsonObj = json_tokener_parse(s);
}

JsonObjectArray::JsonObjectArray(std::string s) {
    mJsonObj = json_tokener_parse(s.c_str());
}

JsonObjectArray::~JsonObjectArray() {
    json_object_put(mJsonObj);
}

int JsonObjectArray::size() {
	return json_object_array_length(mJsonObj);
}

int JsonObjectArray::add(std::string val) {
	return json_object_array_add(mJsonObj, json_tokener_parse(val.c_str()));
}

int JsonObjectArray::put(int idx, std::string val) {
	return json_object_array_put_idx(mJsonObj, idx, json_tokener_parse(val.c_str()));
}

JsonObject JsonObjectArray:: get(int idx) {
	struct json_object* obj = json_object_array_get_idx(mJsonObj, idx);
	if (obj)
		return JsonObject(json_object_get_string(obj));
}

}
