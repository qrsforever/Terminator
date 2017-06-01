#ifndef __Handler__H_
#define __Handler__H_

#include "Result.h"
#include <string>

namespace IBox {

class Handler {
public:
    Handler() : mNextHandler(NULL) { }
	void request(int code, std::string request);
	void setNext(Handler* _handler) { mNextHandler = _handler; }
	virtual void response(int code, std::string request, Result result) = 0;
	virtual Result handleRequest(int code, std::string& request) = 0;
private:
	Handler* mNextHandler;
};

} /* namespace IBox */

#endif // __Handler__H_

