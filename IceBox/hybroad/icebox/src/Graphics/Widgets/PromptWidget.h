#ifndef __PromptWidget__H_
#define __PromptWidget__H_ 

#include "Widget.h"

#include <map>
#include <string>
namespace IBox { 

#define PROMPT_LEVEL_INFO    0
#define PROMPT_LEVEL_ERROR   1
#define PROMPT_LEVEL_WARNING 2

#define PROMPT_TIME_INFINITE 0x000FFFFF
#define PROMPT_CURRENT_MSGID -1

class Mutex;
class PromptWidget : public Widget {
public:
    PromptWidget(int id, int x, int y, int w, int h);

    int attachBy(View* parent);
    int refresh(unsigned int, const SysTime::DateTime&, int);

    ~PromptWidget();

    void hide();
    void show();

    void addMessage(int id, const char* txt, const char* extra, int ts, int align, int level, int number = -1);
    void addMessageI(int id, const char* txt, const char* extra = 0, int ts=1, int align = TextView::ALIGN_CENTER);
    void addMessageW(int id, const char* txt, const char* extra = 0, int ts=2, int align = TextView::ALIGN_CENTER);
    void addMessageE(int id, const char* txt, const char* extra = 0, int ts=3, int align = TextView::ALIGN_CENTER);
    void addMessageX(int id, int number, const char* extra = 0, int ts=1, int align = TextView::ALIGN_CENTER);
    void delMessage(int id, int timeoutflag = 0);

    int getCurrentMsgID();
    std::string getExtraMessage(int id = PROMPT_CURRENT_MSGID);

private:
    void layoutMix();
    class MessageText {
    public:
        MessageText(int id); 
        ~MessageText();
        int msgid;
        int timespec;
        int align;
        int cntdown;
        int level;
        uint32_t endtime;
        std::string text;
        std::string extra;
        MessageText* pre;
        MessageText* next;
    };
private:
    std::map<int, MessageText*> mMapMsg;
    MessageText* mMsg;
    MixNullView* mMix;
    Mutex* mMutex; //for mMsg
    ImageView* mBgImg;
    ImageView* mWarnImg;
    TextView* mMsgTxt;
    bool mIsHiden;
};

}

#endif
