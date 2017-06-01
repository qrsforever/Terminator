#include "libzebra.h"
#include "HWDecryptor.h"

namespace IBox {

unsigned char* HWDecryptor::decrypt(unsigned char* in, int len)
{
	if (in == mInBuf && len <= mBufSize) {
		memset(mOutBuf, 0, mBufSize);
		yresult ret = yhw_security_decryptMem(in, mOutBuf, len, mKey, mKeyLen);
		if (ret == YX_OK) 
			return mOutBuf;
	}	
	return NULL;
}

} // namespace IBox



