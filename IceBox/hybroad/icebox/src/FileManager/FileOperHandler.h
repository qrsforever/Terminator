#ifndef __FileOperHandler__H_
#define __FileOperHandler__H_

#include "Handler.h"
#include "Result.h"

namespace IBox {

class FileOperHandler : public Handler {
public:
	~FileOperHandler();
	static FileOperHandler* getInstance(); 
	static void releaseInstance();

	Result handleRequest(int code, std::string& request);
    void response(int code, std::string request, Result result);
	
private:
	FileOperHandler();
};

} /* namespace IBox */

#endif // __FileOperHandler__H_
