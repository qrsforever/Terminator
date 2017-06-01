#ifndef __UpgradeConfig__H_
#define __UpgradeConfig__H_ 

#include "Setting.h"

namespace IBox { 

class UpgradeConfig : public Setting {
public:
    UpgradeConfig(const char* filename);
    UpgradeConfig(std::string filename);
    ~UpgradeConfig();
    std::string getVersionNumber();
    std::string getSoftwareName();
private:
    void _init();
};


}

#endif
