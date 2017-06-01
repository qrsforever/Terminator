#ifndef __ScanOper__H_
#define __ScanOper__H_

#include "Result.h"
#include "FileOper.h"
#include "Oper.h"

namespace IBox {

class Scan : public Oper {
public:
    Scan(std::string username, std::string topDir);
    void start();
    void accept(std::string path);
    int  fileNumber() { return mFileNumber; }	
    int  folderNumber() { return mFolderNumber; }	
    int  totalFileSize() { return mTotalFileSize; }	
private:
	int 			mFileNumber;
	int 			mFolderNumber;
	size_t			mTotalFileSize;
	std::string		mOperUser;
    std::string     mTopDir;
};

class ScanOper : public FileOper, public FileOperProgressListener {
public:
    ScanOper(int code, std::string request);
	ScanOper(std::string path);
    Result execute();
	void onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize);

private:
	std::string 	mOperUser;
	std::string     mPath;
};

class ScanDisk : public ScanOper {
public:
    ScanDisk(std::string);
	void onFinished(Result result);
};

} // namespace IBox

#endif

