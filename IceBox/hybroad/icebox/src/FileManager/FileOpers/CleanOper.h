#ifndef __Clean__H_
#define __Clean__H_

#include "Oper.h"
#include "Result.h"
#include "FileOper.h"
#include "PermissionManager.h"
#include "FileChangedListener.h"
#include "FileOperProgressListener.h"

namespace IBox {

class Clean : public Oper {
public:
	Clean(std::string operUser, std::string path);
	void start();
	void accept(std::string path);
private:
	std::string		mOperUser;
	std::string 	mTopPath;
};

class CleanOper : public FileOper, public FileOperProgressListener {
public:
    CleanOper(int code, std::string request);
    Result execute();
	void onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize) { ; }
};

class CleanTrashOper : public FileOper, public FileOperProgressListener {
public:
    CleanTrashOper(int code, std::string request);
    Result execute();
	void onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize) { ; }
};

} // namespace IBox

#endif

