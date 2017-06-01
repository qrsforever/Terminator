#ifndef _LayerMixerDevice_H_
#define _LayerMixerDevice_H_

#ifdef __cplusplus

#include <list>

namespace IBox {

class Canvas;
class LCDScreen;
class TVScreen;
class LayerMixerDevice {
public:
    LayerMixerDevice();
    virtual ~LayerMixerDevice();

    virtual void refresh(int screenId, bool force = false);

    int delLCDScreenLayer(LCDScreen* layer);
    int addLCDScreenLayer(LCDScreen* layer); 

    int delTVScreenLayer(TVScreen* layer);
    int addTVScreenLayer(TVScreen* layer); 

protected:
    std::list<LCDScreen*> mLCDLayers; 
    std::list<TVScreen*>  mTVLayers; 
};

} // namespace IBox

#endif // __cplusplus

#endif // _LayerMixerDevice_H_
