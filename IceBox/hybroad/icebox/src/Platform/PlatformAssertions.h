#ifndef __PlatformAssertions__H_
#define __PlatformAssertions__H_

#include "Assertions.h"

#ifdef __cplusplus

extern int gPlatformModuleLevel;

#define PLATFOMR_LOG_ERROR(args...)    LOG_ERROR(HLG_RUNNING, gPlatformModuleLevel, args)
#define PLATFOMR_LOG_WARNING(args...)   LOG_WARNING(HLG_RUNNING, gPlatformModuleLevel, args)
#define PLATFOMR_LOG(args...)   LOG(HLG_RUNNING, gPlatformModuleLevel, args)

#endif //__cplusplus

#endif
