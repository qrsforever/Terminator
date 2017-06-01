#ifndef __FileExplorerThread__H_
#define __FileExplorerThread__H_

#include "Thread.h"

#include <vector>

namespace IBox {

class FileExplorerThread {
public:
    static FileExplorerThread* getInstance(); 

	void init();
	void sendMessage(int what, int arg1, int arg2, const char* data);
	
private:
    FileExplorerThread();
	~FileExplorerThread();
};

void FileExplorerThreadInit();

} /* namespace IBox */

#endif // __FileExplorerThread__H_