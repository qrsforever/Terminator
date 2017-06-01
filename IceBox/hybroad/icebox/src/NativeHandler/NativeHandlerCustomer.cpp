#include "NativeHandlerCustomer.h"
#include "NativeHandlerBoot.h"
#include "NativeHandlerRunning.h"
#include "NativeHandlerUpgrade.h"
#include "NativeHandlerEasysetup.h"
#include "NativeHandlerDiskCopy.h"
#include "Message.h"

namespace IBox {

static NativeHandlerBoot* handleBoot = 0;
static NativeHandlerRunning* handleRunning = 0;
static NativeHandlerUpgrade* handleUpgrade = 0;
static NativeHandlerEasysetup* handleEasysetup = 0;
static NativeHandlerDiskCopy* handleDiskCopy = 0;

void NativeHandlerCustomerInit()
{
    handleBoot = new NativeHandlerBoot;
    handleRunning = new NativeHandlerRunning();
    handleUpgrade = new NativeHandlerUpgrade();
    handleDiskCopy = new NativeHandlerDiskCopy();
    handleEasysetup = new NativeHandlerEasysetup();

    NativeHandler::registerStateHandler(NativeHandler::kBoot, handleBoot);
    NativeHandler::registerStateHandler(NativeHandler::kRunning, handleRunning);
    NativeHandler::registerStateHandler(NativeHandler::kUpgrade, handleUpgrade);
    NativeHandler::registerStateHandler(NativeHandler::kDiskCopy,handleDiskCopy);
    NativeHandler::registerStateHandler(NativeHandler::kEasysetup, handleEasysetup);
}

} // namespace IBox
