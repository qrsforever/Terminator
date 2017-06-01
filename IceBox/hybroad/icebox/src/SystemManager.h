#ifndef _SystemManager_H_
#define _SystemManager_H_

#include "LayerMixerDevice.h"

#ifdef __cplusplus

namespace IBox {

class SystemManager {
public:
    SystemManager();
    ~SystemManager();

    LayerMixerDevice &mixer() { return mMixer; }
    LayerMixerDevice mMixer;
};

SystemManager &systemManager();

} // namespace IBox

#endif // __cplusplus

#endif // _SystemManager_H_
