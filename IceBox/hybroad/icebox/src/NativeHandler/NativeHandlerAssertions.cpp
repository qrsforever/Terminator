#include "NativeHandlerAssertions.h"

#include "LogModule.h"

static int s_nativeHandlerModuleFlag = 0;
int g_nativeHandlerModuleLevel = LOG_LEVEL_WARNING;

namespace IBox {

static LogModule NativeHandlerModule("nativeHandler", s_nativeHandlerModuleFlag, g_nativeHandlerModuleLevel);

} //namespace IBox
