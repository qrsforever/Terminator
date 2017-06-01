#ifndef __EasysetupVIEW__H_
#define __EasysetupVIEW__H_ 

#include "View.h"
#include <string>

namespace IBox {

class EasysetupView : public View {
public:
    EasysetupView(int x, int y, int w, int h);
    ~EasysetupView();

    enum {
        e_PromptQueryMessage = 0,
        e_PromptFailMessage = 1,
        e_PromptSuccessMessage = 2,
        e_PromptDeathMessage = 3,
        e_PromptInterruptMessage = 4,
    };

    void promptQueryMessage(int progress);
    void promptFailMessage(int errorcode);
    void promptSuccessMessage(const char* message);
    void promptDeathMessage();
    void promptInterruptMessage();

protected:
    virtual void onDraw(Canvas*);
    virtual void onHandleInval(const Rect&);

private:
    int mProgress;
    int mErrorCode;
    int mShowWhat;
    std::string mEssid;
    std::string mPassword;
};

}


#endif
