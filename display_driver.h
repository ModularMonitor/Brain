#pragma once

//#define CONFIG_TFT_ILI9488_DRIVER
#include "SPI.h"
#include "TFT/TFT_eSPI.h"
#include "free_fonts.h"

void loop_display(void* arg_unused)
{
    TFT_eSPI tft = TFT_eSPI();
    
    tft.init();
    tft.setRotation(1);

    while(1) {
        size_t frames = 0;
        // 10 sec test
        for(auto until = millis() + 10000; millis() < until;) {
            tft.fillScreen(TFT_WHITE);
            tft.fillScreen(TFT_BLACK);
            ++frames;
            delay(1);
        }

        // Set "cursor" at top left corner of display (0,0) and select font 2
        // (cursor will move to next line automatically during printing with 'tft.println'
        //  or stay on the line is there is room for the text with tft.print)
        tft.setCursor(20, 20, 2);
        // Set the font colour to be white with a black background, set text size multiplier to 1
        tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(4);
        // We can now plot text on screen using the "print" class
        tft.printf("FPS got: %.3f", static_cast<float>(frames) * 0.1f);

        delay(3000);
    }

}