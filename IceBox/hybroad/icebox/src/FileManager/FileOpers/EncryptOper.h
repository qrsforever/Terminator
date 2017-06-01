#ifndef __EncryptOper__H_
#define __EncryptOper__H_

#include "Result.h"
#include "FileOper.h"
#include "Oper.h"

namespace IBox {

class EncryptOper : public FileOper {
public:
    EncryptOper(int code, std::string request);
    Result execute();
};

} // namespace IBox

#endif

