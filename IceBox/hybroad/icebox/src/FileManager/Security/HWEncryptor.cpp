#include "FileExplorerAssertions.h"
#include "libzebra.h"
#include "HWEncryptor.h"

namespace IBox {

unsigned char* HWEncryptor::encrypt(unsigned char* in, int len)
{
	if (in == mInBuf && len <= mBufSize) {
		memset(mOutBuf, 0, mBufSize);	
		yresult ret = yhw_security_encryptMem(in, mOutBuf, len, mKey, mKeyLen);
		if (ret == YX_OK) 
			return mOutBuf;
	}	
	return NULL;
}

} // namespace IBox


