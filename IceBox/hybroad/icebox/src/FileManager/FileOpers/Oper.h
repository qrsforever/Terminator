#ifndef __Oper__H_
#define __Oper__H_

#include "FileFilter.h"
#include "FileNameFilter.h"
#include "PermissionManager.h"
#include "FileChangedListener.h"
#include "FileOperProgressListener.h"

namespace IBox {

class Oper : public FileFilter {	
public:
    Oper() : mFileNameFilter(NULL), mPermissionManager(NULL), mFileChangedListener(NULL), mFileOperProgressListener(NULL) { ; }
	void setFileNameFilter(FileNameFilter* filter) { mFileNameFilter = filter; }	
	void setPermissionManager(PermissionManager* checker) { mPermissionManager = checker; }
	void setFileChangedListener(FileChangedListener* listener) { mFileChangedListener = listener; }
	void setFileOperProgressListener(FileOperProgressListener* listener) { mFileOperProgressListener = listener; }
protected:
	FileNameFilter*				mFileNameFilter;
	PermissionManager*			mPermissionManager;
    FileChangedListener*   		mFileChangedListener;
	FileOperProgressListener*	mFileOperProgressListener;
};

} // namespace IBox

#endif


