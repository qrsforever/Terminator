#ifndef __RemoveOper__H_
#define __RemoveOper__H_

#include "Result.h"
#include "FileOper.h"
#include "Oper.h"

namespace IBox {

class Remove : public Oper {
public:
	Remove(std::string operUser, std::string path);
	void start();
	void accept(std::string path);
private:
	std::string		mOperUser;
	std::string 	mTopPath;
};

class RemoveOper : public FileOper, public FileOperProgressListener {
public:
    RemoveOper(int code, std::string request);
    Result execute();
	void onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize);

private:
	Result			mResult;
};

} // namespace IBox

#endif
