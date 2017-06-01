#ifndef __MakeDiffDBOper__H_
#define __MakeDiffDBOper__H_

#include "Result.h"
#include "FileOper.h"

namespace IBox {

class MakeDiffDBOper : public FileOper {
public:
    MakeDiffDBOper(int code, std::string request);
    Result execute();
};

} // namespace IBox

#endif


