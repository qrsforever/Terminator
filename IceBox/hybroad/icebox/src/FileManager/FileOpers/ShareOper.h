#ifndef __ShareOper__H_
#define __ShareOper__H_

#include "Result.h"
#include "FileOper.h"
#include "Oper.h"

namespace IBox {

class ShareFile : public Oper {	
public:
    ShareFile(std::string operUser, std::string toUser, std::string path, bool share);
    void start();
    void accept(std::string path);
	
private:
	std::string		mOperUser;
	std::string		mToUser;	
    std::string     mTopPath;
    bool     		mShare;	
};

class ShareOper : public FileOper, public FileOperProgressListener {
public:
    ShareOper(int code, std::string request);
    Result execute();
	void onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize);

private:
	Result			mResult;
};

} // namespace IBox

#endif

