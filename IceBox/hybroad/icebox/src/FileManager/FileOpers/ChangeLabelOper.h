#ifndef __ChangeLabelOper__H_
#define __ChangeLabelOper__H_

#include "Result.h"
#include "FileOper.h"

namespace IBox {

class ChangeLabelOper : public FileOper {
public:
    ChangeLabelOper(int code, std::string request);
    Result execute();
};

} // namespace IBox

#endif



