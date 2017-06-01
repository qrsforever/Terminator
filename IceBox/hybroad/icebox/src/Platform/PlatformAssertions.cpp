#include "PlatformAssertions.h"
#include "LogModule.h"

static int s_PlatformModuleFlag = 0;
int gPlatformModuleLevel = LOG_LEVEL_WARNING;

namespace IBox {

static LogModule PlatformModule("platform", s_PlatformModuleFlag, gPlatformModuleLevel);

} // namespace Hippo

