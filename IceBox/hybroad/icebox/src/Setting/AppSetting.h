#ifndef _AppSetting_H_
#define _AppSetting_H_

#ifdef __cplusplus

#include "Setting.h"

namespace IBox {

class AppSetting : public Setting {
public:
	AppSetting(std::string fileName);
	virtual ~AppSetting();

    int recoverBakSeting();
};

AppSetting& appSetting();

} // namespace IBox
#endif //__cplusplus

#endif // _AppSetting_H_
