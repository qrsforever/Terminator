#ifndef _SysSetting_H_
#define _SysSetting_H_

#ifdef __cplusplus

#include "Setting.h"

namespace IBox {

class SysSetting : public Setting {
public:
	SysSetting(std::string fileName);
	virtual ~SysSetting();

    int recoverBakSeting();
};

SysSetting& sysSetting();

} // namespace IBox
#endif //__cplusplus

#endif // _SysSetting_H_
