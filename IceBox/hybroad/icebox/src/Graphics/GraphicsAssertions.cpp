#include "GraphicsAssertions.h"
#include "LogModule.h"

static int s_GraphicsModuleFlag = 0;
int gGraphicsModuleLevel = LOG_LEVEL_ERROR;

namespace IBox {

static LogModule GraphicsModule("graphics", s_GraphicsModuleFlag, gGraphicsModuleLevel);

} // namespace IBox
