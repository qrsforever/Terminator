#ifndef __ChownOper__H_
#define __ChownOper__H_

#include "Result.h"
#include "FileOper.h"
#include "Oper.h"

namespace IBox {

class Chown : public Oper {	
public:
    Chown(std::string operUser, std::string toUser, std::string path);
    void start();
    void accept(std::string path);
private:
	std::string		mOperUser;
	std::string		mToUser;	
    std::string     mTopPath;
};

class ChownOper : public FileOper {
public:
    ChownOper(int code, std::string request);
    Result execute();
};

} // namespace IBox

#endif

