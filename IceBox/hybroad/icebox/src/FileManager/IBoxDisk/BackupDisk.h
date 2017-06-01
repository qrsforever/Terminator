#ifndef __BackupDisk__H_
#define __BackupDisk__H_

#include "VDisk.h"

namespace IBox {

class BackupDisk : public VDisk {
public:
    BackupDisk(std::string topDir);
	~BackupDisk();
	bool isBackupDisk() { return true; }

private:
	
};

} /* namespace IBox */

#endif // __BackupDisk__H_