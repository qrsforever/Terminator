#ifndef __BackupOper__H_
#define __BackupOper__H_

#include "Result.h"
#include "FileOper.h"
#include "Oper.h"
#include "ExcludeFilesFilter.h"

namespace IBox {

class Backup : public Oper {
public:
    Backup(std::string src, std::string dest);
    void start();
    void accept(std::string path);
private:
    std::string     mSrcTopDir;
    std::string     mDestTopDir;
};

class BackupOper {
public:
	static Result backup(); 
};

} // namespace IBox

#endif

