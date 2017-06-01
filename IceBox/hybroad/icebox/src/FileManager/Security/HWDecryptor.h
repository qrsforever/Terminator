#ifndef __HWDecryptor__H_
#define __HWDecryptor__H_

#include "HWSecurity.h"

namespace IBox {

class HWDecryptor : public HWSecurity {
public:
    HWDecryptor(unsigned char* key, int keyLen, int bufSize)
		: HWSecurity(key, keyLen, bufSize) { } 
	//~HWDecryptor();

	unsigned char* decrypt(unsigned char* in, int len);
};

} /* namespace IBox */

#endif // __HWDecryptor__H_


