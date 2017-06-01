#include "FileExplorerAssertions.h"
#include "sha2.h"
#include "libzebra.h"
#include "HWSecurity.h"

namespace IBox {

HWSecurity::HWSecurity(unsigned char* key, int keyLen, int bufSize)
	: mKey(NULL)
	, mKeyLen(16)
	, mBufSize(bufSize)
	, mInBuf(NULL)
	, mOutBuf(NULL)
{
	yhw_mem_alloc(mBufSize, 4, (void **)&mInBuf);
	yhw_mem_alloc(mBufSize, 4, (void **)&mOutBuf);

	unsigned char digest[32] = {0};
	sha256_ctx ctx;
    sha256_init(&ctx);
	sha256_update(&ctx, key, keyLen);
	sha256_final(&ctx, digest);
	mKey = new unsigned char[mKeyLen];
	memcpy(mKey, digest, mKeyLen);
}

HWSecurity::~HWSecurity()
{
	if (mInBuf != NULL)
		yhw_mem_free(mInBuf);
	if (mOutBuf != NULL)
		yhw_mem_free(mOutBuf);
	if (mKey != NULL)
		delete[] mKey;
}

} // namespace IBox


