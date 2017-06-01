#include "XmppAssertions.h"
#include "LogModule.h"

static int s_XmppModuleFlag = 0;
int gXmppModuleLevel = LOG_LEVEL_NORMAL;

namespace IBox {

static LogModule XmppModule("xmpp", s_XmppModuleFlag, gXmppModuleLevel);

} // namespace IBox

