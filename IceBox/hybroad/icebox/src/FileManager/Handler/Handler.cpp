#include "FileExplorerAssertions.h"
#include "Handler.h"

#include <stdio.h>

namespace IBox {

void Handler::request(int code, std::string request)
{
	Result result = handleRequest(code, request);
	if (result != Result::NoHandle) {
		response(code, request, result);
	} else if (mNextHandler) {
		mNextHandler->request(code, request);
	} else {
		logWarn("Can not handle this request: {0x%x, %s}\n", code, request.c_str());
	}	
}	
	
} // namespace IBox
