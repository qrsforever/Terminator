#ifndef __CopyDisk__H_
#define __CopyDisk__H_

#include "VDisk.h"

namespace IBox {

class CopyDisk : public VDisk {
public:
    CopyDisk(std::string topDir);
	~CopyDisk();
	bool isCopyDisk() { return true; }

private:
	
};

} /* namespace IBox */

#endif // __CopyDisk__H_