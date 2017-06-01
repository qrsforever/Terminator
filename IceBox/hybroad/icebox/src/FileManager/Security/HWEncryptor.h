#ifndef __HWEncryptor__H_
#define __HWEncryptor__H_

#include "HWSecurity.h"

namespace IBox {

class HWEncryptor : public HWSecurity {
public:
    HWEncryptor(unsigned char* key, int keyLen, int bufSize) 
		: HWSecurity(key, keyLen, bufSize) { } 
	//~HWEncryptor();

	unsigned char* encrypt(unsigned char* in, int len);
};

} /* namespace IBox */

#endif // __HWEncryptor__H_

