#include <stdio.h>
#include <unistd.h>

#include "Assertions.h"
#include "LogThread.h"
#include "MainThread.h"
#include "NativeHandler.h"
#include "SystemKeyEvent.h"
#include "Setting.h"
#include "PlatformAPI.h"

#include "GraphicsLayout.h"
#include "XmppShell.h"
#include "FileExplorerThread.h"

int main(int argc, char* argv[])
{
    IBox::LogThreadInit();
    IBox::MainThreadInit();

    IBox::Platform().setHDPower(0, true);
    IBox::Platform().setHDPower(1, true);
    IBox::Platform().setUSBPower(0, true);
    IBox::Platform().setUSBPower(1, true);
    IBox::Platform().setFanPower(true);
    IBox::Platform().setFanSpeed(100);
    IBox::Platform().startFan();

    IBox::SettingInitAndLoad();
    IBox::NativeHandlerCreate();
    IBox::SystemKeyEventInit();
	IBox::FileExplorerThreadInit();
    IBox::GraphicsLayoutInit();

    IBox::MainThreadRun();

    SYSTEM_LOG_ERROR("Never run here!\n");
    return 0;
}
