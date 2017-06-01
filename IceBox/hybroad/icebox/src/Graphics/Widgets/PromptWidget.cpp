#include "PromptWidget.h"
#include "WeatherManager.h"
#include "MessageTypes.h"

#include "Mutex.h"

#define INFO_MSG_LOCX 10
#define INFO_MSG_LOCY 5
#define INFO_MSG_W ((320 - 2*INFO_MSG_LOCX))
#define INFO_MSG_H ((80 - 2*INFO_MSG_LOCY))

#define WARN_MSG_LOCX 55
#define WARN_MSG_LOCY 5
#define WARN_MSG_W ((320 - 2*WARN_MSG_LOCX))
#define WARN_MSG_H ((80 - 2*WARN_MSG_LOCY) - 10)

namespace IBox {

PromptWidget::PromptWidget(int id, int x, int y, int w, int h) 
    : Widget(id, x, y, w, h), mMsg(0), mIsHiden(true)
{
    mMix = new MixNullView(x, y, w, h);
    mMutex = new Mutex();
    layoutMix();
}

PromptWidget::~PromptWidget()
{
    if (mMix) {
        mMix->detachFromParent();
        delete mBgImg;
        delete mWarnImg;
        delete mMsgTxt;
        delete mMix;
    }
}

void 
PromptWidget::layoutMix()
{
    if (mMix) {
        mBgImg = new ImageView(5, 5, 310, 70);
        mBgImg->setSource(PROMPT_ICON_BG);

        mWarnImg = new ImageView(15, 20, 30, 30);
        mWarnImg->setSource(PROMPT_ICON_WARNING);

        mMsgTxt = new TextView(15, 5, 290, 70);
        mMsgTxt->setFontSize(28);
        // mMsgTxt->setFontColor(0xff, 0xff, 0xff, 0xff);
        // mMsgTxt->setAlignStyle(TextView::ALIGN_LEFT_CENTER);

        mMix->attachChildToFront(mBgImg);
        mMix->attachChildToFront(mWarnImg);
        mMix->attachChildToFront(mMsgTxt);

        mBgImg->setVisibleP(true);
        // mWarnImg->setVisibleP(true);
        mMsgTxt->setVisibleP(true);
    }
}

int 
PromptWidget::attachBy(View* parent)
{
    if (parent) {
        parent->attachChildToFront(mMix);
    }
    return 0;
}

void 
PromptWidget::hide()
{
    if (!mIsHiden)
        mMix->hide();
    mIsHiden = true;
}

void 
PromptWidget::show()
{
    if (mIsHiden)
        mMix->show();
    mIsHiden = false;
}

int 
PromptWidget::refresh(unsigned int clocktime, const SysTime::DateTime& dt, int force)
{
    if (!mMsg) {
        this->hide();
        return 0;
    }

    MessageText* msg = mMsg;
    if (msg->cntdown < 0) {
        if (!msg->endtime) {
            if (msg->timespec < 0)
                msg->endtime = clocktime + PROMPT_TIME_INFINITE * 1000;
            else
                msg->endtime = clocktime + msg->timespec * 1000;
            // mMsgTxt->setFontSize(28);
            mMsgTxt->setFontColor(0xff, 0xff, 0xff, 0xff);
            if (msg->level == PROMPT_LEVEL_INFO) {
                mWarnImg->setVisibleP(false);
                mBgImg->setImageAlpha(0.9);
                mMsgTxt->setLoc(INFO_MSG_LOCX, INFO_MSG_LOCY);
                mMsgTxt->setSize(INFO_MSG_W, INFO_MSG_H);
                mMsgTxt->setAlignStyle(msg->align);
            } else {
                mWarnImg->setVisibleP(true);
                mBgImg->setImageAlpha(1.0);
                mMsgTxt->setLoc(WARN_MSG_LOCX, WARN_MSG_LOCY);
                mMsgTxt->setSize(WARN_MSG_W, WARN_MSG_H);
                mMsgTxt->setAlignStyle(TextView::ALIGN_LEFT_CENTER);
            }
            mMsgTxt->setText(msg->text.c_str()).inval(0);
            this->show();
            return 1;
        }
        if (clocktime > msg->endtime) {
            //TODO
            delMessage(msg->msgid, 1/*timeout*/);
        }
    } else {
        char buff[16] = { };
        snprintf(buff, 15, "%d", msg->cntdown);

        if (0 == msg->cntdown) {
            delMessage(msg->msgid, 1);
            this->hide();
            return 0;
        }
        msg->cntdown -= 1;

        mWarnImg->setVisibleP(false);
        mBgImg->setImageAlpha(0.9);
        // mMsgTxt->setFontSize(28);
        mMsgTxt->setFontColor(0x00, 0xff, 0x00, 0xff);
        mMsgTxt->setLoc(INFO_MSG_LOCX, INFO_MSG_LOCY);
        mMsgTxt->setSize(INFO_MSG_W, INFO_MSG_H);
        mMsgTxt->setAlignStyle(msg->align);
        mMsgTxt->setText(buff).inval(0);
        this->show();
        return 1;
    }
    return 0;
}

int 
PromptWidget::getCurrentMsgID()
{
    int msgid = -1;
    mMutex->lock();
    if (mMsg)
        msgid = mMsg->msgid;
    mMutex->unlock();
    return msgid;
}

std::string 
PromptWidget::getExtraMessage(int id)
{
    MessageText* msg = 0;
    mMutex->lock();
    if (id == PROMPT_CURRENT_MSGID) {
        msg = mMsg;
    } else {
        std::map<int, MessageText*>::iterator it = mMapMsg.find(id);
        if (it != mMapMsg.end())
            msg = it->second;
    }
    if (msg) {
        mMutex->unlock();
        return msg->extra;
    }
    mMutex->unlock();
    return "";
}

void 
PromptWidget::addMessageI(int id, const char* txt, const char* extra, int ts, int align)
{
    return addMessage(id, txt, extra, ts, align, PROMPT_LEVEL_INFO);
}

void 
PromptWidget::addMessageW(int id, const char* txt, const char* extra, int ts, int align)
{
    return addMessage(id, txt, extra, ts, align, PROMPT_LEVEL_WARNING);
}

void 
PromptWidget::addMessageE(int id, const char* txt, const char* extra, int ts, int align)
{
    return addMessage(id, txt, extra, ts, align, PROMPT_LEVEL_ERROR);
}

void 
PromptWidget::addMessageX(int id, int number, const char* extra, int ts, int align)
{
    return addMessage(id, 0, extra, ts, align, PROMPT_LEVEL_INFO, number);
}

void 
PromptWidget::addMessage(int id, const char* txt, const char* extra, int ts, int align, int level, int number)
{
    std::map<int, MessageText*>::iterator it = mMapMsg.find(id);
    if (it != mMapMsg.end())
        return;

    MessageText* node = new MessageText(id);
    node->timespec = ts;
    node->text = txt ? txt : "";
    node->extra = extra ? extra : "";
    node->align = align;
    node->level = level;
    node->cntdown = number;

    mMapMsg.insert(std::make_pair(id, node));
    if (mHandler)
        mHandler->sendMessage(mHandler->obtainMessage(MessageType_Prompt, MV_PROMPT_ADD, id));

    mMutex->lock();
    MessageText* first = mMsg;
    if (first) {
        node->next = first;
        node->pre = first->pre;
        first->pre->next = node;
        first->pre = node;
    } else 
        mMsg = node;
    mMutex->unlock();
    return ;
}

void 
PromptWidget::delMessage(int id, int flag)
{
    std::map<int, MessageText*>::iterator it = mMapMsg.find(id);
    if (it == mMapMsg.end())
        return;

    mMutex->lock();
    MessageText* msg = it->second;
    if (msg->next == msg && msg->pre == msg) {
        mMsg = 0;
        mMapMsg.erase(msg->msgid);
        if (mHandler)
            mHandler->sendMessage(mHandler->obtainMessage(MessageType_Prompt, (flag ? MV_PROMTP_TIMEOUT : MV_PROMPT_REMOVE), msg->msgid));
        delete msg;
    } else {
        if (msg == mMsg) 
            mMsg = msg->next;
        msg->next->pre = msg->pre;
        msg->pre->next = msg->next;
        mMapMsg.erase(msg->msgid);
        if (mHandler)
            mHandler->sendMessage(mHandler->obtainMessage(MessageType_Prompt, (flag ? MV_PROMTP_TIMEOUT : MV_PROMPT_REMOVE), msg->msgid));
        delete msg;
    }
    mMutex->unlock();
}

PromptWidget::MessageText::MessageText(int id) : msgid(id), endtime(0), cntdown(-1) 
{ 
    pre = next = this; 
} 

PromptWidget::MessageText::~MessageText()
{
}

}
