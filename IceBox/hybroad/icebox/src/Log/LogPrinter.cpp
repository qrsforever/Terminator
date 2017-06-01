#include "LogPrinter.h"

#include <stdio.h>

namespace IBox {
bool
LogPrinter::pushBlock(uint8_t* blockHead, uint32_t blockLength)
{
    fwrite(blockHead, blockLength, 1, stdout);
    return false;
}

} // namespace IBox
