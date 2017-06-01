#include "GraphicsAssertions.h"
#include "SlideTransition.h"
#include "UncoverRight.h"
#include "unistd.h"

namespace IBox {

SlideTransition::SlideTransition(DisplayScreen& screen)
    : mScreen(screen), mShowInterval(5/*s*/)
    , mTaskState(-1), mSlideState(-1)
{
    mSlideBG = new SlideImage(mScreen.locX(), mScreen.locY(), mScreen.width(), mScreen.height());
    mSlideBG->setDisplayMode(e_DISPLAY_FULLSCREEN); /* background must be fullscreen */
    mSlideBG->setVisibleP(true);
    mScreen.attachChildToFront(mSlideBG);
}

SlideTransition::~SlideTransition()
{
    if (mSlideBG)
        delete mSlideBG;
    mPictures.clear();
}

int 
SlideTransition::insert(const char* picpath)
{
    if (picpath)
        mPictures.push_back(picpath);
}

int 
SlideTransition::start(int mode, int effect)
{
    if (mTaskState == _Task::_eRunning)
        return 0; 
    mDisplayMode = mode;
    mEffectType = effect;
    (new _Task(*this))->start();
    mSlideState = e_START;
    return 0;
}

int 
SlideTransition::stop()
{
    mSlideState = e_STOP;
    return 0;
}

void 
SlideTransition::_Task::run()
{
    GRAPHICS_LOG("SlideTransition thread run\n");
    _Owner.mTaskState = _eRunning;
    {
        DisplayScreen& screen = _Owner.mScreen;
        SlideImage* image = new SlideImage(screen.locX(), screen.locY(), screen.width(), screen.height());
        image->setDisplayMode(_Owner.mDisplayMode);
        image->setVisibleP(true);
        screen.attachChildToFront(image);
        int cnt = _Owner.mPictures.size();
        GRAPHICS_LOG("image run\n");
        for (int i = 0; i < cnt; ++i) {
            GRAPHICS_LOG("image %s\n", _Owner.mPictures[i].c_str());
            image->setSource(_Owner.mPictures[i].c_str());
            image->startEffect(_Owner.mEffectType); //will block
            for (int j = 0; j < _Owner.mShowInterval; ++j) {
                if (_Owner.mSlideState == e_STOP)
                    break;
                sleep(1);
            }
            _Owner.mSlideBG->setSource(_Owner.mPictures[i].c_str()).inval(0);
            if (_Owner.mSlideState == e_STOP)
                break;
        }
        GRAPHICS_LOG("image end\n");
        image->detachFromParent();
        delete image;
    }
    _Owner.mTaskState = _eFinish;
    GRAPHICS_LOG("SlideTransition thread end\n");
    delete this;
}

}
