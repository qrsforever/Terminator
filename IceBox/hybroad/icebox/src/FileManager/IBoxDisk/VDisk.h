#ifndef __VDisk__H_
#define __VDisk__H_

#include <string>

namespace IBox {

class VDisk {
public:
    VDisk(std::string topDir);
	~VDisk();
	virtual bool isLocalDisk() { return false; }
	virtual bool isRemovableDisk() { return false; }
	virtual bool isBackupDisk() { return false; }
	virtual bool isCopyDisk() { return false; }
	virtual std::string diskName() { return "Disk"; }
	
	std::string getTopDir() { return mTopDir; }		
	void setDevName(std::string dev) { mDevName = dev; }
	std::string getDevName() { return mDevName; }
	void mkDirs(std::string path);
		
private:
	std::string mTopDir;
	std::string mDevName;
};

} /* namespace IBox */

#endif // __VDisk__H_

