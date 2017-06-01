#ifndef __HWSecurity__H_
#define __HWSecurity__H_

namespace IBox {

class HWSecurity {
public:
    HWSecurity(unsigned char* key, int keyLen, int bufSize);
	~HWSecurity();

	unsigned char* getInputBuffer() { return mInBuf; }
	virtual unsigned char* encrypt(unsigned char* in, int len) { return NULL; }
	virtual unsigned char* decrypt(unsigned char* in, int len) { return NULL; }
	
protected:
	int	  mKeyLen;
	unsigned char* mKey;

	int   mBufSize;
	unsigned char* mInBuf;
	unsigned char* mOutBuf;
};

} /* namespace IBox */

#endif // __HWSecurity__H_

