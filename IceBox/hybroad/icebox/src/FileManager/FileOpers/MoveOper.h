#ifndef __MoveOper__H_
#define __MoveOper__H_

#include "Result.h"
#include "FileOper.h"
#include "Oper.h"

namespace IBox {

class Move : public Oper {
public:
	Move(std::string operUser, std::string srcPath, std::string destPath);
	void start();
	void accept(std::string path);
private:
	std::string		mOperUser;
	std::string     mSrcTopDir;
    std::string     mDestTopDir;
};

class MoveOper : public FileOper, public FileOperProgressListener {
public:
    MoveOper(int code, std::string request);
    Result execute();
	void onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize);

private:
	Result			mResult;
};

} // namespace IBox

#endif
