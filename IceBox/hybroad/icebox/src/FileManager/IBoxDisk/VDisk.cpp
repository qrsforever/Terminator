#include "File.h"
#include "VDisk.h"

namespace IBox {

VDisk::VDisk(std::string topDir)
	: mTopDir(topDir)
{
		
}

VDisk::~VDisk()
{
		
}

void VDisk::mkDirs(std::string path) 
{
    File dir(path);
    if (!dir.exists()) {
        File parentDir(dir.getParent());
        if (!parentDir.exists())
            mkDirs(parentDir.path());
        dir.mkDir();
    }
}
	
} /* namespace IBox */