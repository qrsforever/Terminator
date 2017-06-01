#include "GraphicsAssertions.h"
#include "EasysetupView.h"
#include "JsonObject.h"
#include "Canvas.h"
#include <stdlib.h>

#define ES_QUERY_START      "请配置网络"
#define ES_QUERY_FAIL       "网络查询失败"
#define ES_QUERY_AGAIN      "请按C键重新查询"
#define ES_QUERY_SUCCESS    "收到网络配置信息"
#define ES_WIFI             "WIFI"
#define ES_PASSWORD         "密码"
#define ES_CONFIRM_RESULT   "确认请按C键"
#define ES_DEATH_WAKEUP     "云的世界安静了"
#define ES_INTERRUPT_WAKEUP "网络配置被中断"

namespace IBox {

EasysetupView::EasysetupView(int x, int y, int w, int h)
{
    setLoc(x, y);
    setSize(w, h);
}

EasysetupView::~EasysetupView()
{

}

void 
EasysetupView::promptQueryMessage(int progress)
{
    mShowWhat = e_PromptQueryMessage;
    mProgress = progress;
    inval(0);
}

void 
EasysetupView::promptFailMessage(int errcode)
{
    mShowWhat = e_PromptFailMessage;
    mErrorCode = errcode;
    inval(0);
}

void 
EasysetupView::promptSuccessMessage(const char* message)
{
    if (isJsonString(message)) {
        JsonObject obj(message);
        mEssid = obj.getString("Essid");
        mPassword = obj.getString("Password");
        mShowWhat = e_PromptSuccessMessage;
        inval(0);
    }
}

void 
EasysetupView::promptDeathMessage()
{
    mShowWhat = e_PromptDeathMessage;
    inval(0);
}

void 
EasysetupView::promptInterruptMessage()
{
    mShowWhat = e_PromptInterruptMessage;
    inval(0);
}

void 
EasysetupView::onDraw(Canvas* canvas)
{
    cairo_text_extents_t extents;
    double x = 0, y = 0;

    cairo_t* cr = canvas->mCairo;

    cairo_save(cr); 
    {
        cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);                  
        cairo_paint(cr);
        cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);        

        char buff[128] = { 0 };
        if (mShowWhat == e_PromptQueryMessage) {
            switch (mProgress % 4) {
            case 0:
                snprintf(buff, 127, "%s", ES_QUERY_START);
                break;
            case 1:
                snprintf(buff, 127, "%s .", ES_QUERY_START);
                break;
            case 2:
                snprintf(buff, 127, "%s . .", ES_QUERY_START);
                break;
            case 3:
                snprintf(buff, 127, "%s . . .", ES_QUERY_START);
                break;
            }
            cairo_set_font_size(cr, 38);
            cairo_text_extents(cr, buff, &extents);
            x = 40; //(width() >> 1) - (extents.width/2 + extents.x_bearing);
            y = (height() >> 1 ) - (extents.height/2 + extents.y_bearing);
            cairo_move_to(cr, x, y);
            cairo_show_text (cr, buff);
        } else if (mShowWhat == e_PromptFailMessage) {
            cairo_translate (cr, 0, 25);
            cairo_set_font_size(cr, 25);
            snprintf(buff, 127, "%s: %d", ES_QUERY_FAIL, mErrorCode);
            cairo_text_extents(cr, buff, &extents);
            x = 25;
            y = -extents.y_bearing;
            cairo_move_to(cr, x, y);
            cairo_show_text (cr, buff);

            cairo_translate (cr, 0, y + 25);
            cairo_set_source_rgb (cr, 1, 1, 0);
            snprintf(buff, 127, "%s?", ES_QUERY_AGAIN);
            cairo_text_extents (cr, buff, &extents);
            x = 25; 
            y = -extents.y_bearing;
            cairo_move_to (cr, x, y);
            cairo_show_text (cr, buff);
        } else if (mShowWhat == e_PromptSuccessMessage) {
            cairo_translate (cr, 0, 5);
            cairo_set_font_size(cr, 22);
            snprintf(buff, 127, "%s:", ES_QUERY_SUCCESS);
            cairo_text_extents(cr, buff, &extents);
            x = 15;
            y = -extents.y_bearing;
            cairo_move_to(cr, x, y);
            cairo_show_text (cr, buff);

            cairo_translate (cr, 0, y + 13);
            snprintf(buff, 127, "%s: %s", ES_WIFI, mEssid.c_str());
            cairo_text_extents (cr, buff, &extents);
            x = 45; 
            y = -extents.y_bearing;
            cairo_move_to (cr, x, y);
            cairo_show_text (cr, buff);

            cairo_translate (cr, 0, y + 10);
            snprintf(buff, 127, "%s: %s", ES_PASSWORD, mPassword.c_str());
            cairo_text_extents (cr, buff, &extents);
            x = 45; 
            y = -extents.y_bearing;
            cairo_move_to (cr, x, y);
            cairo_show_text (cr, buff);

            cairo_translate (cr, 0, y + 13);
            cairo_set_source_rgb (cr, 1, 1, 0);
            snprintf(buff, 127, "%s", ES_CONFIRM_RESULT);
            cairo_text_extents (cr, buff, &extents);
            x = 15; 
            y = -extents.y_bearing;
            cairo_move_to (cr, x, y);
            cairo_show_text (cr, buff);
        } else if (mShowWhat == e_PromptDeathMessage) {
            snprintf(buff, 127, "%s!", ES_DEATH_WAKEUP);
            cairo_set_font_size(cr, 38);
            cairo_text_extents(cr, buff, &extents);
            x = 20;
            y = (height() >> 1 ) - (extents.height/2 + extents.y_bearing);
            cairo_move_to(cr, x, y);
            cairo_show_text (cr, buff);
        } else if (mShowWhat == e_PromptInterruptMessage) {
            snprintf(buff, 127, "%s!", ES_INTERRUPT_WAKEUP);
            cairo_set_font_size(cr, 38);
            cairo_text_extents(cr, buff, &extents);
            x = 20;
            y = (height() >> 1 ) - (extents.height/2 + extents.y_bearing);
            cairo_move_to(cr, x, y);
            cairo_show_text (cr, buff);
        }
    }
    cairo_restore(cr);
}

void
EasysetupView::onHandleInval(const Rect& r)
{
}

}
