#ifndef __ViewLayout__H_
#define __ViewLayout__H_

#include "DesktopScreen.h"
#include "DisplayScreen.h"
#include "PromptWidget.h"
#include "UPSStatusWidget.h"
#include "SlideTransition.h"

#define ICON_AVG    40
#define ICON_WIDTH  40
#define ICON_HEIGHT 50

#define FAN_STATUS_ICON_X  (0 * ICON_AVG) 
#define FAN_STATUS_ICON_Y  0 
#define FAN_STATUS_ICON_W  ICON_WIDTH
#define FAN_STATUS_ICON_H  ICON_HEIGHT

#define HD0_STATUS_ICON_X  (1 * ICON_AVG)
#define HD0_STATUS_ICON_Y  0 
#define HD0_STATUS_ICON_W  ICON_WIDTH
#define HD0_STATUS_ICON_H  ICON_HEIGHT

#define HD1_STATUS_ICON_X  (2 * ICON_AVG)
#define HD1_STATUS_ICON_Y  0 
#define HD1_STATUS_ICON_W  ICON_WIDTH
#define HD1_STATUS_ICON_H  ICON_HEIGHT

#define SD_STATUS_ICON_X   (3 * ICON_AVG)
#define SD_STATUS_ICON_Y   0 
#define SD_STATUS_ICON_W   ICON_WIDTH
#define SD_STATUS_ICON_H   ICON_HEIGHT

#define USB0_STATUS_ICON_X  (4 * ICON_AVG)
#define USB0_STATUS_ICON_Y  0 
#define USB0_STATUS_ICON_W  ICON_WIDTH
#define USB0_STATUS_ICON_H  ICON_HEIGHT

#define USB1_STATUS_ICON_X  (5 * ICON_AVG)
#define USB1_STATUS_ICON_Y  0
#define USB1_STATUS_ICON_W  ICON_WIDTH
#define USB1_STATUS_ICON_H  ICON_HEIGHT

#define WIFI_STATUS_ICON_X  (6 * ICON_AVG)
#define WIFI_STATUS_ICON_Y  0 
#define WIFI_STATUS_ICON_W  ICON_WIDTH
#define WIFI_STATUS_ICON_H  ICON_HEIGHT

#define UPS_STATUS_ICON_X   (7 * ICON_AVG)
#define UPS_STATUS_ICON_Y   0 
#define UPS_STATUS_ICON_W   ICON_WIDTH
#define UPS_STATUS_ICON_H   ICON_HEIGHT

#define TIME_TEXT_X 0
#define TIME_TEXT_Y 50
#define TIME_TEXT_W 160
#define TIME_TEXT_H 120

#define WEATHER_MIX_X 160
#define WEATHER_MIX_Y 50
#define WEATHER_MIX_W 160
#define WEATHER_MIX_H 120

#define DATE_TEXT_X 0
#define DATE_TEXT_Y 170
#define DATE_TEXT_W 160
#define DATE_TEXT_H 70

#define AQI_MIX_X 160 
#define AQI_MIX_Y 170
#define AQI_MIX_W 160
#define AQI_MIX_H 70

#define PROMPT_X 0 
#define PROMPT_Y 160
#define PROMPT_W 320
#define PROMPT_H 80


namespace IBox {

void GraphicsLayoutInit();

DesktopScreen& Desktop();
DisplayScreen& Display();

PromptWidget& Prompt();

UPSStatusWidget& UPSWidget();
Widget& DiskWidget(std::string label);
}
#endif
