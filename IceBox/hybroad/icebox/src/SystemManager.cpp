#include "SystemManager.h"

namespace IBox {

SystemManager::SystemManager()
{
}

SystemManager::~SystemManager()
{
}

static SystemManager gSystemManager;

SystemManager &systemManager()
{
    return gSystemManager;
}
} // namespace IBox

