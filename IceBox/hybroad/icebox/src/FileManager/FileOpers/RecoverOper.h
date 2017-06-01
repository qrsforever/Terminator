#ifndef __RecoverOper__H_
#define __RecoverOper__H_

#include "Result.h"
#include "FileOper.h"
#include "Oper.h"

namespace IBox {

class Recover : public Oper {	
public:
    Recover(std::string operUser, std::string path);
    void start();
    void accept(std::string path);
private:
	std::string		mOperUser;	
    std::string     mTopPath;
};

class RecoverOper : public FileOper, public FileOperProgressListener {
public:
    RecoverOper(int code, std::string request);
    Result execute();
	void onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize);

private:
	Result			mResult;
};

} // namespace IBox

#endif

