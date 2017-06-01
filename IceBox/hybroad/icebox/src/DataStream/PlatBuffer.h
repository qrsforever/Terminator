#ifndef _PlatBuffer_H_
#define _PlatBuffer_H_

#include <stdint.h>

#ifdef __cplusplus

namespace IBox {

class PlatBuffer {
public:
    PlatBuffer(uint32_t size);
    PlatBuffer(uint8_t *bufHead, uint32_t bufLength);
    ~PlatBuffer();

    uint8_t *getBufHead() { return mBufHead; }
    uint32_t getBufLength() { return mBufLength; }

private:
	bool mFreeBySelf;
    uint8_t *mBufHead;
    uint32_t mBufLength;
};

} /* namespace IBox */

#endif /* __cplusplus */

#endif /* _PlatBuffer_H_ */
