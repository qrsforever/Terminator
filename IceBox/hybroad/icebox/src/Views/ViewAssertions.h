#ifndef _ViewAssertions_H_
#define _ViewAssertions_H_

#include "Assertions.h"

extern int g_viewModuleLevel;

#ifdef __cplusplus

namespace IBox {

#define VIEW_LOG_ERROR(args...) LOG_ERROR(HLG_RUNNING, g_viewModuleLevel, args)
#define VIEW_LOG_WARNING(args...) LOG_WARNING(HLG_RUNNING, g_viewModuleLevel, args)
#define VIEW_LOG(args...) LOG(HLG_RUNNING, g_viewModuleLevel, args)

} // namespace IBox

#endif //__cplusplus

#endif //_ViewAssertions_H_

