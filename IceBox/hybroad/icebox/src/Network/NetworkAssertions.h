#ifndef __NetworkAssertions__H_
#define __NetworkAssertions__H_

#include "Assertions.h"

#ifdef __cplusplus

extern int gNetworkModuleLevel;

#define NETWORK_LOG_ERROR(args...)    LOG_ERROR(HLG_RUNNING, gNetworkModuleLevel, args)
#define NETWORK_LOG_WARNING(args...)   LOG_WARNING(HLG_RUNNING, gNetworkModuleLevel, args)
#define NETWORK_LOG(args...)   LOG(HLG_RUNNING, gNetworkModuleLevel, args)

#endif //__cplusplus

#endif
