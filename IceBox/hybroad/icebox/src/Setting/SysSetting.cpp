#include "Assertions.h"
#include "SysSetting.h"

namespace IBox {

static SysSetting g_sysSetting("/root/.hy_sys.ini");

SysSetting::SysSetting(std::string fileName)
    : Setting(fileName)
{
}

SysSetting::~SysSetting()
{
}

int
SysSetting::recoverBakSeting()
{
    return 0;
}

SysSetting& sysSetting()
{
    return g_sysSetting;
}

} // namespace IBox
