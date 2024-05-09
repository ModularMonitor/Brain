#pragma once

//#define CONFIG_TFT_ILI9488_DRIVER
#include "SPI.h"
#include "TFT/TFT_eSPI.h"
#include "free_fonts.h"
#include "display_class.h"
#include "devices_data_bridge.h"

void loop_display(void* arg_useless)
{
    Display disp;

    while(1) {
        delay(1);
        disp.think_and_draw();
    }
}




//      pinMode(TOUCH_CS, OUTPUT);
//      
//      TFT_eSPI tft = TFT_eSPI();
//      uint16_t x, y;
//      //uint16_t calibrationData[5];
//      
//      tft.init();
//      tft.setRotation(3);
//      tft.fillScreen((0xFFFF));
//      
//      //tft.calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 30);
//      
//      tft.fillScreen((0xFFFF));
//  
//      float fps = 0.0f;
//  
//      while(1) {
//          delay(1);
//          //size_t frames = 100;
//          // 10 sec test
//          //for(auto until = millis() + 10000; millis() < until;) {
//          //    tft.fillScreen(TFT_WHITE);
//          //    tft.fillScreen(TFT_BLACK);
//          //    ++frames;
//          //    delay(1);
//          //}
//  
//          const auto nw = micros();
//          tft.fillScreen(TFT_WHITE);
//          tft.fillScreen(TFT_BLACK);
//          const auto l8 = micros();
//  
//          fps = (fps * 9.0f + (1.0f / (float(l8 - nw) * 0.000001f))) / 10.0f;
//  
//          // Set "cursor" at top left corner of display (0,0) and select font 2
//          // (cursor will move to next line automatically during printing with 'tft.println'
//          //  or stay on the line is there is room for the text with tft.print)
//          tft.setCursor(340, 290, 2);
//          // Set the font colour to be white with a black background, set text size multiplier to 1
//          tft.setTextColor(TFT_WHITE,TFT_BLACK);
//          tft.setTextSize(2);
//          // We can now plot text on screen using the "print" class
//          tft.printf("FPS: %.2f", fps);
//  
//          //delay(5000);
//          //tft.fillScreen(TFT_BLACK);
//          
//          //static uint16_t color;
//  
//          //for(auto until = millis() + 30000; millis() < until;) {
//          //tft.getTouchRaw(&x, &y);
//          //z = tft.getTouchRawZ();
//          if (tft.getTouch(&x, &y)) {
//              tft.setCursor(0, 290, 2);
//              tft.setTextColor(TFT_WHITE,TFT_BLACK);
//              tft.setTextSize(2);
//              tft.printf("M %hu:%hu", x, y);
//          }
//              //if (tft.getTouch(&x, &y)) {
//              //    tft.setCursor(5, 5, 2);
//              //    tft.printf("x: %i     ", x);
//              //    tft.setCursor(5, 20, 2);
//              //    tft.printf("y: %i    ", y);
//  //
//              //    tft.drawPixel(x, y, color);
//              //    color += 155;
//              //}
//  
//              //delay(1);
//          //}
//      }
//  
//     