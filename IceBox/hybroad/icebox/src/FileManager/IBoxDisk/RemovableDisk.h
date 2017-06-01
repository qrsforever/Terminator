#ifndef __RemovableDisk__H_
#define __RemovableDisk__H_

#include "VDisk.h"

namespace IBox {

class RemovableDisk : public VDisk {
public:
    RemovableDisk(std::string topDir);
	~RemovableDisk();
	bool isRemovableDisk() { return true; }
	std::string diskName() { return "Removable Disk"; }
private:
	
};

} /* namespace IBox */

#endif // __RemovableDisk__H_