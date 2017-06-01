#ifndef __AutomaticSTATENOTIFY__H_
#define __AutomaticSTATENOTIFY__H_

#include "StateNotify.h"
#include "Setting.h"

#ifdef __cplusplus

namespace IBox {
#include <string>

class AutomaticSetting : public Setting {
public :
    AutomaticSetting(std::string filename);
    virtual ~AutomaticSetting();
    std::string get(const char* name);
};

class AutomaticStateNotify : public StateNotify {
public:
    AutomaticStateNotify(NetworkInterface* iface);
                        
    ~AutomaticStateNotify();
    virtual int notify(int state);
};

}
#endif

#endif
