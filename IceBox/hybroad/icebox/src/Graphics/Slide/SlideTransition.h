#ifndef __SlideTransition__H_
#define __SlideTransition__H_

#include "Thread.h"
#include "DisplayScreen.h"
#include "SlideImage.h"

#include <vector>
#include <string>

namespace IBox {

class SlideTransition {
public:
    friend class _Task;
    SlideTransition(DisplayScreen& screen);
    ~SlideTransition();

    enum {
        e_START = 1,
        e_STOP  = 2,
    };

    int insert(const char* picpath);
    int start(int mode = e_DISPLAY_OPTIMALRATIO, int type = e_EFFECT_FADEOUTSMOOTHLY);
    int stop();

    class _Task : public Thread {
    public :
        enum {
            _eRunning = 1,
            _eFinish = 2,
        };
        _Task(SlideTransition& slide) : _Owner(slide) { }
        ~_Task() { };
        virtual void run();
    private:
        SlideTransition& _Owner;
    };

private:
    int mSlideState;
    int mTaskState;
    int mShowInterval;
    int mDisplayMode;
    int mEffectType;
    SlideImage* mSlideBG;
    std::vector<std::string> mPictures;
    DisplayScreen& mScreen;
};

}


#endif

