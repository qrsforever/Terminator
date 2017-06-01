#include "Assertions.h"
#include "LayerMixerDevice.h"

#include "libzebra.h"
#include "LCDScreen.h"
#include "TVScreen.h"

#define SCREEN_LCD  1
#define SCREEN_TV   2

namespace IBox {

LayerMixerDevice::LayerMixerDevice()
{
}

LayerMixerDevice::~LayerMixerDevice()
{
}


int 
LayerMixerDevice::delLCDScreenLayer(LCDScreen* layer)
{
    if (!layer)
        return -1;
    mLCDLayers.remove(layer);
    return 0;
}

int
LayerMixerDevice::addLCDScreenLayer(LCDScreen* layer)
{
    if (!layer)
        return -1;
    std::list<LCDScreen*>::iterator it;
    for (it = mLCDLayers.begin(); it != mLCDLayers.end(); ++it) { 
        if (layer->Zorder() < (*it)->Zorder())
            break;
    }
    mLCDLayers.insert(it, layer);
    return 0;
}

int 
LayerMixerDevice::delTVScreenLayer(TVScreen* layer)
{
    if (!layer)
        return -1;
    mTVLayers.remove(layer);
    return 0;
}

int
LayerMixerDevice::addTVScreenLayer(TVScreen* layer)
{
    if (!layer)
        return -1;
    std::list<TVScreen*>::iterator it;
    for (it = mTVLayers.begin(); it != mTVLayers.end(); ++it) { 
        if (layer->Zorder() < (*it)->Zorder())
            break;
    }
    mTVLayers.insert(it, layer);
    return 0;
}

void
LayerMixerDevice::refresh(int screenId, bool force)
{
    bool repainted = force;

    if (SCREEN_LCD == screenId) {
        bool r = false;
        std::list<LCDScreen*>::iterator it;
        Rect r1;
        Rect r2;
        r1.setEmpty();
        for (it = mLCDLayers.begin(); it != mLCDLayers.end(); ++it) {
            r = (*it)->repaint(&r2);
            if (r) {
                r1.join(r2);
                repainted = true;
            }
        }
        if (repainted) {
            yhw_lcd_updateDisplay(r1.fLeft, r1.fTop, r1.width(), r1.height());
        }
    } else if (SCREEN_TV == screenId) {
        std::list<TVScreen*>::iterator it;
        for (it = mTVLayers.begin(); it != mTVLayers.end(); ++it) {
            repainted |= (*it)->repaint();
        }

        if (repainted) {
            ygp_layer_updateScreen();
        }
    }
}

} // namespace IBox
