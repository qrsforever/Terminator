#ifndef _Types_H_
#define _Types_H_

#include <stdio.h>

#ifdef __cplusplus

namespace IBox {

/*
Noncopyable is the base class for objects that may do not want to
be copied. It hides its copy-constructor and its assignment-operator.
*/
class Noncopyable {
public:
    Noncopyable() {}
    
private:
    Noncopyable(const Noncopyable&);
    Noncopyable& operator=(const Noncopyable&);
};

} // namespace IBox

#endif // __cplusplus

#endif // _Types_H_
