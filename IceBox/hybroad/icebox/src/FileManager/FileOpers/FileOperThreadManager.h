#ifndef __FileOperThreadManager__H_
#define __FileOperThreadManager__H_

#include "FileOper.h"
#include "FileOperThread.h"
#include "OperThreadStateListener.h"

#include <vector>

namespace IBox {

class FileOperThreadManager : public OperThreadStateListener {
public:
    static FileOperThreadManager* getInstance(); 
    
    void execute(FileOper*);
	void onBusy();
	void onIdle();
	
private:
	std::vector<FileOper*> mFileOpers;	
	std::vector<FileOperThread*> mThreads;
    FileOperThreadManager();
	~FileOperThreadManager();
};

void FileOperThreadManagerInit();

} /* namespace IBox */

#endif // __FileOperThreadManager__H_