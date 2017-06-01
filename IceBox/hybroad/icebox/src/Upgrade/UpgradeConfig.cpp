#include "UpgradeAssertions.h"
#include "UpgradeConfig.h"

#include <string.h>
#include <string>

namespace IBox { 

UpgradeConfig::UpgradeConfig(const char* filename) : Setting(filename)
{
    _init();
}

UpgradeConfig::UpgradeConfig(std::string filename) : Setting(filename.c_str())
{
    _init();
}

UpgradeConfig::~UpgradeConfig()
{
}

void 
UpgradeConfig::_init()
{
    add("IceBox.Version", "-1");
    add("IceBox.Software", "0");
}

std::string
UpgradeConfig::getVersionNumber()
{
    char buffer[128] = { 0 };
    Setting::get("IceBox.Version", buffer, 127);
    return buffer;
}

std::string 
UpgradeConfig::getSoftwareName()
{
    char buffer[128] = { 0 };
    Setting::get("IceBox.Software", buffer, 127);
    return buffer;
}

}
