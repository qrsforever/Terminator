#ifndef __FileExplorerAssertions__H_
#define __FileExplorerAssertions__H_

#include "Assertions.h"

#ifdef __cplusplus

extern int gFileExplorerModuleLevel;

#define FILEEXPLORER_LOG_ERROR(args...)    LOG_ERROR(HLG_RUNNING, gFileExplorerModuleLevel, args)
#define FILEEXPLORER_LOG_WARNING(args...)   LOG_WARNING(HLG_RUNNING, gFileExplorerModuleLevel, args)
#define FILEEXPLORER_LOG(args...)   LOG(HLG_RUNNING, gFileExplorerModuleLevel, args)

#define logMsg	FILEEXPLORER_LOG
#define logErr	FILEEXPLORER_LOG_ERROR
#define logWarn FILEEXPLORER_LOG_WARNING

#endif //__cplusplus

#endif
