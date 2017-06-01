#include "GraphicsAssertions.h"
#include "GraphicsLayout.h"

#include "LCDLayer.h"
#include "TVLayer.h"
#include "SystemManager.h"

#include "Canvas.h"
#include "libzebra.h"

#include "DiskDeviceManager.h"

#include "FanStatusWidget.h"
#include "HDiskStatusWidget.h"
#include "SDCardStatusWidget.h"
#include "USBStatusWidget.h"
#include "WifiStatusWidget.h"
#include "UPSStatusWidget.h"
#include "DateWidget.h"
#include "TimeWidget.h"
#include "AQIWidget.h"
#include "WeatherWidget.h"

#include "ManualStateNotify.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "ResourcePathDefine.h"
#include "SysTime.h"

#include "ft2build.h"
#include "cairo-ft.h"
#include "freetype/freetype.h"


#define FONT_FACE_PATH "/var/MicroHei.ttf"

FT_Face share_ft_face;

namespace IBox {

DesktopScreen*  gDS = 0;
DisplayScreen*  gPD = 0;

void GraphicsLayoutInit()
{
    GRAPHICS_LOG("GraphicsLayoutInit()\n");
    if (gDS)
        return;

    static FT_Library gLibrary;
    FT_Init_FreeType(&gLibrary);
    FT_New_Face(gLibrary, FONT_FACE_PATH,  0, &share_ft_face);

    LCDLayer& lcdLayer = LCDLayer::self();

    gDS = new DesktopScreen(lcdLayer.mCanvas, lcdLayer.mWidth, lcdLayer.mHeight, 1);
    // gDS->setBackgroud(DESKTOP_ICON_DIR"app.png");

    gDS->addWidget(new FanStatusWidget(FAN_WIDGET_ID, FAN_STATUS_ICON_X, FAN_STATUS_ICON_Y, FAN_STATUS_ICON_W, FAN_STATUS_ICON_H));
    gDS->addWidget(new HDiskStatusWidget(HD0_WIDGET_ID, HD0_STATUS_ICON_X, HD0_STATUS_ICON_Y, HD0_STATUS_ICON_W, HD0_STATUS_ICON_H));
    gDS->addWidget(new HDiskStatusWidget(HD1_WIDGET_ID, HD1_STATUS_ICON_X, HD1_STATUS_ICON_Y, HD1_STATUS_ICON_W, HD1_STATUS_ICON_H));
    gDS->addWidget(new SDCardStatusWidget(SD_WIDGET_ID, SD_STATUS_ICON_X, SD_STATUS_ICON_Y, SD_STATUS_ICON_W, SD_STATUS_ICON_H));
    gDS->addWidget(new USBStatusWidget(USB0_WIDGET_ID, USB0_STATUS_ICON_X, USB0_STATUS_ICON_Y, USB0_STATUS_ICON_W, USB0_STATUS_ICON_H));
    gDS->addWidget(new USBStatusWidget(USB1_WIDGET_ID, USB1_STATUS_ICON_X, USB1_STATUS_ICON_Y, USB1_STATUS_ICON_W, USB1_STATUS_ICON_H));
    gDS->addWidget(new WifiStatusWidget(WIFI_WIDGET_ID, WIFI_STATUS_ICON_X, WIFI_STATUS_ICON_Y, WIFI_STATUS_ICON_W, WIFI_STATUS_ICON_H));
    gDS->addWidget(new UPSStatusWidget(UPS_WIDGET_ID, UPS_STATUS_ICON_X, UPS_STATUS_ICON_Y, UPS_STATUS_ICON_W, UPS_STATUS_ICON_H));
    
    gDS->addWidget(new TimeWidget(TIME_WIDGET_ID, TIME_TEXT_X, TIME_TEXT_Y, TIME_TEXT_W, TIME_TEXT_H));
    gDS->addWidget(new WeatherWidget(WEATHER_WIDGET_ID, WEATHER_MIX_X, WEATHER_MIX_Y, WEATHER_MIX_W, WEATHER_MIX_H));
    gDS->addWidget(new DateWidget(DATE_WIDGET_ID, DATE_TEXT_X, DATE_TEXT_Y, DATE_TEXT_W, DATE_TEXT_H));
    gDS->addWidget(new AQIWidget(AQI_WIDGET_ID, AQI_MIX_X, AQI_MIX_Y, AQI_MIX_W, AQI_MIX_H));

    gDS->addWidget(new PromptWidget(PROMPT_WIDGET_ID, PROMPT_X, PROMPT_Y, PROMPT_W, PROMPT_H));

    gDS->showMainLayer();

    systemManager().mixer().addLCDScreenLayer(gDS);

    //TODO
    NativeHandler& H = defNativeHandler();
    H.sendMessageDelayed(H.obtainMessage(MessageType_DesktopRefresh, 0, 0, 0), 1000); 

    TVLayer& tvLayer = TVLayer::self();
    gPD = new DisplayScreen(tvLayer.mCanvas, tvLayer.mWidth, tvLayer.mHeight, 1);
    // gPD->setBackgroud(DISPLAY_PICS_DIR"pic0.jpg");

    gPD->inval(0);
    gPD->setVisibleP(true);

    systemManager().mixer().addTVScreenLayer(gPD);

    //TODO test
    gPD->slide().insert(TEST_SLIDE_PIC0);
    gPD->slide().insert(TEST_SLIDE_PIC1);
    gPD->slide().insert(TEST_SLIDE_PIC2);
    gPD->slide().insert(TEST_SLIDE_PIC3);
    gPD->slide().insert(TEST_SLIDE_PIC4);
    gPD->slide().insert(TEST_SLIDE_PIC5);
    gPD->slide().insert(TEST_SLIDE_PIC6);


    Prompt().setMessageHandler(&H);
}

DesktopScreen& Desktop()
{
    if (!gDS) {
        GraphicsLayoutInit();
    }
    return *gDS;
}

DisplayScreen& Display()
{ 
    if (!gPD) {
        GraphicsLayoutInit();
    }
    return *gPD;
}

PromptWidget& Prompt()
{
    return *(static_cast<PromptWidget*>(gDS->getWidget(PROMPT_WIDGET_ID)));
}

UPSStatusWidget& UPSWidget()
{
    return *(static_cast<UPSStatusWidget*>(gDS->getWidget(UPS_WIDGET_ID)));
}

Widget& DiskWidget(std::string label)
{
    if (!label.compare(BUSLABEL_HDD0))
        return *gDS->getWidget(HD0_WIDGET_ID);
    if (!label.compare(BUSLABEL_HDD1))
        return *gDS->getWidget(HD1_WIDGET_ID);
    if (!label.compare(BUSLABEL_USB0))
        return *gDS->getWidget(USB0_WIDGET_ID);
    if (!label.compare(BUSLABEL_USB1))
        return *gDS->getWidget(USB1_WIDGET_ID);
    return *gDS->getWidget(SD_WIDGET_ID);
}

}
