
#include "PlatBuffer.h"

#include <stdlib.h>


namespace IBox {

PlatBuffer::PlatBuffer(uint32_t size)
	: mFreeBySelf(true)
{
    mBufHead = (uint8_t *)malloc(size);
    mBufLength = size;
}

PlatBuffer::PlatBuffer(uint8_t *bufHead, uint32_t bufLength)
	: mFreeBySelf(false)
	, mBufHead(bufHead)
	, mBufLength(bufLength)
{
}

PlatBuffer::~PlatBuffer()
{
    if (mFreeBySelf)
        free(mBufHead);
}

} /* namespace IBox */
