#include "FileExplorerAssertions.h"
#include "LogModule.h"

static int s_FileExplorerModuleFlag = 0;
int gFileExplorerModuleLevel = LOG_LEVEL_WARNING;

namespace IBox {

static LogModule FileExplorerModule("fileexplorer", s_FileExplorerModuleFlag, gFileExplorerModuleLevel);

} // namespace IBox
