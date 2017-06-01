#ifndef __MakeDirOper__H_
#define __MakeDirOper__H_

#include "Result.h"
#include "FileOper.h"
#include "Oper.h"

namespace IBox {

class MakeDir : public Oper {
public:
    MakeDir(std::string operUser, std::string path);
    void start();
	void accept(std::string path) { ; }
private:
	std::string     mOperUser;
	std::string 	mPath;
    void mkDirs(std::string path);
};

class MakeDirOper : public FileOper, public FileOperProgressListener {
public:
    MakeDirOper(int code, std::string request);
    Result execute();
	void onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize);

private:
	Result			mResult;
};

class MakeDefDirsOper : public FileOper {
public:
    MakeDefDirsOper(std::string path);
    Result execute();
private:
	std::string mTopPath;
};

class MakeUserDirsOper : public FileOper {
public:
    MakeUserDirsOper(std::string username, std::string path);
    Result execute();
private:
	std::string mOperUser;
	std::string mTopPath;
};

} // namespace IBox

#endif

