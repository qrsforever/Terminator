#ifndef __CopyOper__H_
#define __CopyOper__H_

#include "Result.h"
#include "FileOper.h"
#include "Oper.h"
#include "FileNameFilter.h"
#include "FileChangedListener.h"
#include "PermissionManager.h"
#include "FileOperProgressListener.h"

namespace IBox {

class Copy : public Oper {
public:
    Copy(std::string user, std::string src, std::string dest);
    void start();
    void accept(std::string path);

private:
	int mTotalNumber;
	int mCopiedNumber;
	size_t  mTotalSize;
	size_t	mCopiedSize;
	std::string     mOperUser;
    std::string     mSrcTopDir;
    std::string     mDestTopDir;

	void onProgressUpdate(int copiedNum, size_t copiedSize);
};

class CopyOper : public FileOper, public FileOperProgressListener {
public:
    CopyOper(int code, std::string request);
    Result execute();
	void onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize);
};

class CopyDiskOper : public FileOper, public FileOperProgressListener {
public:
    CopyDiskOper(std::string, std::string);
	Result execute();
	void onFinished(Result result) {  }
	void onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize);
private:
	size_t mTotalSize;
	size_t mCopiedSize;
	std::string mDiskName;
	std::string mDestPath;	
};

} // namespace IBox

#endif
