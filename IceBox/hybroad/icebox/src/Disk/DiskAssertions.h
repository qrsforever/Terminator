#ifndef __DiskAssertions__H_
#define __DiskAssertions__H_

#include "Assertions.h"

#ifdef __cplusplus

extern int gDiskModuleLevel;

#define DISK_LOG_ERROR(args...)    LOG_ERROR(HLG_RUNNING, gDiskModuleLevel, args)
#define DISK_LOG_WARNING(args...)   LOG_WARNING(HLG_RUNNING, gDiskModuleLevel, args)
#define DISK_LOG(args...)   LOG(HLG_RUNNING, gDiskModuleLevel, args)

#endif //__cplusplus

#endif
