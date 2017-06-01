#ifndef __OperThreadStateListener__H_
#define __OperThreadStateListener__H_

namespace IBox {

class OperThreadStateListener {
public:	
	virtual void onBusy() = 0;
	virtual void onIdle() = 0;
};

} // namespace IBox

#endif
