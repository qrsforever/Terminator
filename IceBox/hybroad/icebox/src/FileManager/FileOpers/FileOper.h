#ifndef __FileOper__H_
#define __FileOper__H_

#include "Result.h"
#include "Object.h"
#include <string>

namespace IBox {

class FileOper : public Object {
public:
	FileOper(int code = 0, std::string request = "");
    void start();
    virtual Result execute() = 0;
	virtual void onFinished(Result result);
protected:
	int 		mOperCode;
	std::string mOperRequest;    
};

} /* namespace IBox */

#endif // __FileOper__H_

