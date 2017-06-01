#include "ViewAssertions.h"

#include "LogModule.h"

static int s_viewModuleFlag = 0;
int g_viewModuleLevel = LOG_LEVEL_WARNING;

namespace IBox {

static LogModule ViewLogModule("view", s_viewModuleFlag, g_viewModuleLevel);

} // namespace IBox
