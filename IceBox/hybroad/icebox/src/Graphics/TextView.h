#ifndef __TEXTVIEW__H_
#define __TEXTVIEW__H_ 

#include "LCDScreen.h"

#include <string>

namespace IBox {

class TextView : public View {
public:
    TextView(int x, int y, int w, int h);
    ~TextView();

    enum {
       ALIGN_LEFT_TOP = 1,
       ALIGN_LEFT_CENTER = 2, 
       ALIGN_LEFT_BOTTOM = 3,

       ALIGN_RIGHT_TOP = 4,
       ALIGN_RIGHT_CENTER = 5, 
       ALIGN_RIGHT_BOTTOM = 6,

       ALIGN_CENTER = 100,
       ALIGN_CENTER_BOTTOM = 101,
    };

    void setBGColor(int r, int g, int b, int a = 0);
    void setFontColor(int r, int g, int b, int a = 255);
    void setFontSize(int size) { mFontSize = size; }
    void setAlignStyle(int style) { mAlign = style; } 
    TextView& setText(const char* text);

protected:
    virtual void onDraw(Canvas*);
    virtual void onHandleInval(const Rect&);
private:
    int mFontSize;
    int mAlign;
    ColorAttr mFontColor;
    ColorAttr mBGColor;
    std::string mText;
};

}


#endif
