#include "DiskAssertions.h"
#include "LogModule.h"

static int s_DiskModuleFlag = 0;
int gDiskModuleLevel = LOG_LEVEL_WARNING;

namespace IBox {

static LogModule DiskModule("disk", s_DiskModuleFlag, gDiskModuleLevel);

} // namespace IBox

