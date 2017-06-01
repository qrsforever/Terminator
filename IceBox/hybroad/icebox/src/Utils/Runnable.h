#ifndef _RUNNABLE_H_
#define _RUNNABLE_H_

#include "Object.h"

#ifdef __cplusplus

namespace IBox {

class Runnable : public Object {
public:
    Runnable() {};
    virtual ~Runnable() {};
    virtual void run() = 0;
};

} /* namespace IBox */

#endif // __cplusplus

#endif /* _RUNNABLE_H_ */
