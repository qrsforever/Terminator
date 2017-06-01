#ifndef __GraphicsAssertions__H_
#define __GraphicsAssertions__H_

#include "Assertions.h"

#ifdef __cplusplus

extern int gGraphicsModuleLevel;

#define GRAPHICS_LOG_ERROR(args...)    LOG_ERROR(HLG_RUNNING, gGraphicsModuleLevel, args)
#define GRAPHICS_LOG_WARNING(args...)   LOG_WARNING(HLG_RUNNING, gGraphicsModuleLevel, args)
#define GRAPHICS_LOG(args...)   LOG(HLG_RUNNING, gGraphicsModuleLevel, args)

#endif //__cplusplus

#endif
