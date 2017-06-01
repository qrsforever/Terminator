#include "UpgradeAssertions.h"
#include "LogModule.h"

static int s_UpgradeModuleFlag = 0;
int gUpgradeModuleLevel = LOG_LEVEL_WARNING;

namespace IBox {

static LogModule UpgradeModule("upgrade", s_UpgradeModuleFlag, gUpgradeModuleLevel);

} // namespace Hippo

