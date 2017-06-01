#ifndef __FileOperProgressListener__H_
#define __FileOperProgressListener__H_

#include <string>

namespace IBox {

class FileOperProgressListener {
public:	
	virtual void onProgressUpdate(int totalNum, size_t totalSize, int copiedNum, size_t copiedSize) = 0;
};

} // namespace IBox

#endif
