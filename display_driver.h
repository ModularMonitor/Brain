#pragma once

//#define CONFIG_TFT_ILI9488_DRIVER
#include "SPI.h"
#include "TFT/TFT_eSPI.h"

void loop_display(void* arg_unused)
{
    TFT_eSPI tft = TFT_eSPI();
    
    tft.init();
    tft.setRotation(1);

    while(1) {
        tft.fillScreen(TFT_WHITE);
        delay(500);
        tft.fillScreen(TFT_BLACK);
        delay(500);
    }

}