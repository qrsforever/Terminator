#ifndef __AuthCodeVIEW__H_
#define __AuthCodeVIEW__H_ 

#include "View.h"
#include <string>

namespace IBox {

class AuthCodeView : public View {
public:
    AuthCodeView(int x, int y, int w, int h);
    ~AuthCodeView();

    AuthCodeView& setCodeString(const char* string);
    AuthCodeView& setCodeNumber(int code);

    std::string getCodeString();
    int getCodeNumber();
protected:
    virtual void onDraw(Canvas*);
    virtual void onHandleInval(const Rect&);
private:
    int mLabelFontSize;
    int mCodeFontSize;
    std::string mCodeString;
};

}


#endif
