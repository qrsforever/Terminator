#ifndef __CalculateHashCodeOper__H_
#define __CalculateHashCodeOper__H_

#include "Result.h"
#include "FileOper.h"
#include "Oper.h"
#include "ExcludeFilesFilter.h"

namespace IBox {

class CalculateHashCode : public Oper {
public:
    CalculateHashCode(std::string path);
    void start();
    void accept(std::string path);
private:
    std::string     mTopDir;
};

class CalculateHashCodeOper {
public:
	static Result calculate(); 
};

} // namespace IBox

#endif


