#ifndef __UpgradeAssertions__H_
#define __UpgradeAssertions__H_

#include "Assertions.h"

#ifdef __cplusplus

extern int gUpgradeModuleLevel;

#define UPGRADE_LOG_ERROR(args...)    LOG_ERROR(HLG_RUNNING, gUpgradeModuleLevel, args)
#define UPGRADE_LOG_WARNING(args...)  LOG_WARNING(HLG_RUNNING, gUpgradeModuleLevel, args)
#define UPGRADE_LOG(args...)          LOG(HLG_RUNNING, gUpgradeModuleLevel, args)

#endif //__cplusplus

#endif
