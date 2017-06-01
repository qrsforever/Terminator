#ifndef __XmppAssertions__H_
#define __XmppAssertions__H_

#include "Assertions.h"

#ifdef __cplusplus

extern int gXmppModuleLevel;

#define XMPP_LOG_ERROR(args...)    LOG_ERROR(HLG_RUNNING, gXmppModuleLevel, args)
#define XMPP_LOG_WARNING(args...)  LOG_WARNING(HLG_RUNNING, gXmppModuleLevel, args)
#define XMPP_LOG(args...)          LOG(HLG_RUNNING, gXmppModuleLevel, args)

#endif //__cplusplus

#endif
