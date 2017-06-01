#ifndef _NativeHandlerAssertions_H_
#define _NativeHandlerAssertions_H_

#include "Assertions.h"

extern int g_nativeHandlerModuleLevel;

#ifdef __cplusplus

namespace IBox {

#define NATIVEHANDLER_LOG_ERROR(args...) LOG_ERROR(HLG_RUNNING, g_nativeHandlerModuleLevel, args)
#define NATIVEHANDLER_LOG_WARNING(args...) LOG_WARNING(HLG_RUNNING, g_nativeHandlerModuleLevel, args)
#define NATIVEHANDLER_LOG(args...) LOG(HLG_RUNNING, g_nativeHandlerModuleLevel, args)
#define NATIVEHANDLER_LOG_VERBOSE(args...) LOG_VERBOSE(HLG_RUNNING, g_nativeHandlerModuleLevel, args)

} // namespace IBox

#endif //__cplusplus

#endif //_NativeHandlerAssertions_H_
