#ifndef __DECLARE_ENUM__H_
#define __DECLARE_ENUM__H_

namespace IBox {

#define DECLARE_ENUM(E)							\
class E {										\
public:											\
    E(int value = 0) : mValue((__Enum)value) { 	\
    } 											\
    E& operator=(int value) {					\
        mValue = (__Enum)value;					\
        return *this; 							\
    } 											\
    operator int() const {						\
        return mValue; 							\
    } 											\
												\
    enum __Enum {

#define END_ENUM() 								\
    }; 											\
												\
private:										\
    __Enum mValue;								\
};

} /* namespace IBox */

#endif

