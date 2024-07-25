#pragma once

#include "Arduino.h"

namespace Bitmaps {
    // 11 x 13 SD card icon:
    PROGMEM const unsigned char sd_card_icon_11_13[] = {
        0b00011111, 0b11100000,
        0b00111111, 0b11100000,
        0b01111111, 0b11100000,
        0b11111000, 0b11100000,
        0b11110000, 0b11100000,
        0b11100000, 0b11100000,
        0b11100000, 0b11100000,
        0b11100000, 0b11100000,
        0b11100000, 0b11100000,
        0b11100000, 0b11100000,
        0b11111111, 0b11100000,
        0b11111111, 0b11100000,
        0b11111111, 0b11100000
    };

    // 11 x 14 DB icon
    /*PROGMEM const unsigned char database_icon_11_14[] = {
        0b01111111, 0b11000000,
        0b11111111, 0b11100000,
        0b11100000, 0b11100000,
        0b11111111, 0b11100000,
        0b11111111, 0b11100000,
        0b11011111, 0b01100000,
        0b11000000, 0b01100000,
        0b11000000, 0b01100000,
        0b11000000, 0b01100000,
        0b11000000, 0b01100000,
        0b11100000, 0b11100000,
        0b11111111, 0b11100000,
        0b01111111, 0b11000000,
        0b00111111, 0b10000000            
    };*/

    // 40 x 46 icon home
    PROGMEM const unsigned char config_icon_home[] = {
        0x00, 0x01, 0xF8, 0x00, 0x00, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x00, 0x1F, 
        0xFF, 0x80, 0x00, 0x00, 0x7F, 0xFF, 0xE0, 0x00, 0x01, 0xFF, 0xFF, 0xF8, 
        0x00, 0x07, 0xFF, 0xFF, 0xFE, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0x80, 0x7F, 
        0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 
        0xFF, 0xF0, 0xFC, 0x00, 0x00, 0x03, 0xF0, 0x70, 0x00, 0x00, 0x00, 0xE0, 
        0x0E, 0x00, 0x00, 0x07, 0x00, 0x1F, 0x00, 0x00, 0x0F, 0x80, 0x1F, 0x00, 
        0x00, 0x0F, 0x80, 0x1F, 0x00, 0x00, 0x0F, 0x80, 0x1F, 0x00, 0x00, 0x0F, 
        0x80, 0x1F, 0x00, 0x00, 0x0F, 0x80, 0x1F, 0x00, 0x00, 0x0F, 0x80, 0x1F, 
        0x00, 0xFF, 0xCF, 0x80, 0x1F, 0x00, 0xFF, 0xCF, 0x80, 0x1F, 0x00, 0xFF, 
        0xCF, 0x80, 0x1F, 0x00, 0xFE, 0x0F, 0x80, 0x1F, 0x00, 0xFF, 0x0F, 0x80, 
        0x1F, 0x00, 0xFF, 0x8F, 0x80, 0x1F, 0x00, 0xEF, 0xCF, 0x80, 0x1F, 0x00, 
        0xE3, 0xCF, 0x80, 0x1F, 0x00, 0xE1, 0xEF, 0x80, 0x1F, 0x00, 0xE1, 0xF7, 
        0x80, 0x1F, 0x00, 0x00, 0xF7, 0x80, 0x1F, 0x00, 0x00, 0xFB, 0x80, 0x1F, 
        0xFF, 0xFF, 0x7B, 0x80, 0x1F, 0xFF, 0xFF, 0x7D, 0x80, 0x1F, 0xFF, 0xFF, 
        0xBD, 0x80, 0x1F, 0xFF, 0xFF, 0xBD, 0x80, 0x0F, 0xFF, 0xFF, 0xBD, 0x00, 
        0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 
        0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x03, 0xF0, 
        0x00, 0x00, 0x70, 0x07, 0xE0, 0x00, 0x00, 0x7C, 0x3F, 0xC0, 0x00, 0x00, 
        0x7F, 0xFF, 0x80, 0x00, 0x00, 0x3F, 0xFE, 0x00, 0x00, 0x00, 0x07, 0xF8, 
        0x00, 0x00
    };
    // 26 x 35
    PROGMEM const unsigned char config_icon_up[] = {
        0x00, 0x7F, 0x80, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x01, 0xFF, 0xE0, 0x00, 
        0x01, 0xFF, 0xE0, 0x00, 0x03, 0xFF, 0xF0, 0x00, 0x07, 0xFF, 0xF8, 0x00, 
        0x07, 0xFF, 0xF8, 0x00, 0x0F, 0xFF, 0xFC, 0x00, 0x0F, 0xBF, 0x7C, 0x00, 
        0x1F, 0xBF, 0x7E, 0x00, 0x1F, 0x3F, 0x3E, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 
        0x7E, 0x3F, 0x1F, 0x80, 0x7E, 0x3F, 0x1F, 0x80, 0xFC, 0x3F, 0x0F, 0xC0, 
        0xF8, 0x3F, 0x07, 0xC0, 0xF8, 0x3F, 0x07, 0xC0, 0x70, 0x3F, 0x03, 0x80, 
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00
    };
    // 26 x 35
    PROGMEM const unsigned char config_icon_down[] = {
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
        0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x70, 0x3F, 0x03, 0x80, 
        0xF8, 0x3F, 0x07, 0xC0, 0xF8, 0x3F, 0x07, 0xC0, 0xFC, 0x3F, 0x0F, 0xC0, 
        0x7E, 0x3F, 0x1F, 0x80, 0x7E, 0x3F, 0x1F, 0x80, 0x3F, 0x3F, 0x3F, 0x00, 
        0x1F, 0x3F, 0x3E, 0x00, 0x1F, 0xBF, 0x7E, 0x00, 0x0F, 0xBF, 0x7C, 0x00, 
        0x0F, 0xFF, 0xFC, 0x00, 0x07, 0xFF, 0xF8, 0x00, 0x07, 0xFF, 0xF8, 0x00, 
        0x03, 0xFF, 0xF0, 0x00, 0x01, 0xFF, 0xE0, 0x00, 0x01, 0xFF, 0xE0, 0x00, 
        0x00, 0xFF, 0xC0, 0x00, 0x00, 0x7F, 0x80, 0x00
    };
    

    PROGMEM const unsigned char qr_code_icon[] = {
        0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0xFE, 
        0x60, 0x44, 0x92, 0x06, 0x60, 0x53, 0x66, 0x06, 0x60, 0x6C, 0x9A, 0x06, 
        0x60, 0x77, 0x76, 0x06, 0x60, 0x6B, 0x6A, 0x06, 0x60, 0x50, 0x86, 0x06, 
        0x7F, 0xFF, 0x6F, 0xFE, 0x61, 0xFF, 0x93, 0x2E, 0x7B, 0x07, 0xE8, 0xE6, 
        0x67, 0x00, 0xEF, 0xB6, 0x6D, 0xE0, 0x33, 0x3E, 0x77, 0xFC, 0x1C, 0xEE, 
        0x6C, 0xFF, 0x1C, 0xAE, 0x68, 0x1F, 0x8F, 0xEE, 0x7C, 0x07, 0x8E, 0x3E, 
        0x6F, 0x83, 0xCE, 0xC6, 0x7F, 0xE1, 0xC7, 0xF6, 0x7F, 0xF1, 0xC7, 0x6E, 
        0x71, 0xF8, 0xE6, 0x4E, 0x60, 0x78, 0xE7, 0xFE, 0x78, 0x3C, 0xE6, 0x06, 
        0x7E, 0x1C, 0x66, 0x06, 0x7F, 0x1C, 0x7E, 0x06, 0x67, 0x1E, 0xDA, 0x06, 
        0x43, 0x9F, 0xE2, 0x06, 0x43, 0xBA, 0x56, 0x06, 0x67, 0xFF, 0xFF, 0xFE, 
        0x7F, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00
        //0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 
        //0x06, 0x22, 0x49, 0x60, 0x06, 0xCA, 0x66, 0x60, 0x06, 0x36, 0x59, 0x60, 
        //0x06, 0xEE, 0x6E, 0x60, 0x06, 0xD6, 0x56, 0x60, 0x06, 0x0A, 0x61, 0x60, 
        //0xFE, 0xFF, 0xF6, 0x7F, 0x86, 0xFF, 0xC9, 0x74, 0xDE, 0xE0, 0x17, 0x67, 
        //0xE6, 0x00, 0xF7, 0x6D, 0xB6, 0x07, 0xCC, 0x7C, 0xEE, 0x3F, 0x38, 0x77, 
        //0x36, 0xFF, 0x38, 0x75, 0x16, 0xF8, 0xF1, 0x77, 0x3E, 0xE0, 0x71, 0x7C, 
        //0xF6, 0xC1, 0x73, 0x63, 0xFE, 0x87, 0xE3, 0x6F, 0xFE, 0x8F, 0xE3, 0x76, 
        //0x8E, 0x1F, 0x67, 0x72, 0x06, 0x1E, 0xE7, 0x7F, 0x1E, 0x3C, 0x67, 0x60, 
        //0x7E, 0x38, 0x66, 0x60, 0xFE, 0x38, 0x7E, 0x60, 0xE6, 0x78, 0x5B, 0x60, 
        //0xC2, 0xF9, 0x47, 0x60, 0xC2, 0x5D, 0x6A, 0x60, 0xE6, 0xFF, 0xFF, 0x7F, 
        //0xFE, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00
    };

    // 32 x 38
    //PROGMEM const unsigned char config_icon_debug[] = {
    //    0x1F, 0xFF, 0xFF, 0xF8, 0x7F, 0xFF, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0xFE, 
    //    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xFF, 0xFF, 0xFF, 
    //    0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF, 
    //    0xFE, 0x07, 0xFF, 0xFF, 0xFF, 0x03, 0xFF, 0xFF, 0xFF, 0xC3, 0xFF, 0xFF, 
    //    0xFF, 0xC3, 0xFF, 0xFF, 0xFF, 0x03, 0xFF, 0xFF, 0xFE, 0x07, 0xFF, 0xFF, 
    //    0xF8, 0x1F, 0xFF, 0xFF, 0xF0, 0x3F, 0xC0, 0x0F, 0xF0, 0xFF, 0xC0, 0x0F, 
    //    0xF1, 0xFF, 0xC0, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    //    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    //    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    //    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    //    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    //    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFE, 
    //    0x7F, 0xFF, 0xFF, 0xFE, 0x1F, 0xFF, 0xFF, 0xF8
    //};
    // 32 x 32
    PROGMEM const unsigned char config_icon_config[] = {
        0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 
        0x03, 0x07, 0xE0, 0xC0, 0x07, 0x87, 0xE1, 0xE0, 0x0F, 0xCF, 0xF3, 0xF0, 
        0x1F, 0xFF, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF, 0xF8, 0x0F, 0xFF, 0xFF, 0xF0, 
        0x07, 0xFF, 0xFF, 0xE0, 0x03, 0xFC, 0x3F, 0xC0, 0x03, 0xF0, 0x0F, 0xC0, 
        0x07, 0xE0, 0x07, 0xE0, 0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0xC0, 0x03, 0xFF, 
        0xFF, 0xC0, 0x03, 0xFF, 0xFF, 0xC0, 0x03, 0xFF, 0xFF, 0xC0, 0x03, 0xFF, 
        0xFF, 0xE0, 0x07, 0xFF, 0x07, 0xE0, 0x07, 0xE0, 0x03, 0xF0, 0x0F, 0xC0, 
        0x03, 0xFC, 0x3F, 0xC0, 0x07, 0xFF, 0xFF, 0xE0, 0x0F, 0xFF, 0xFF, 0xF0, 
        0x1F, 0xFF, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF, 0xF8, 0x0F, 0xCF, 0xF3, 0xF0, 
        0x07, 0x87, 0xE1, 0xE0, 0x03, 0x07, 0xE0, 0xC0, 0x00, 0x07, 0xE0, 0x00, 
        0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00
    };
    // 24 x 24 icon close
    //PROGMEM const unsigned char terminal_icon_close[] = {
    //    0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFE, 0x63, 0xFF, 0xC6, 
    //    0x61, 0xFF, 0x86, 0x60, 0xFF, 0x06, 0x70, 0x7E, 0x0E, 0x78, 0x3C, 0x1E, 
    //    0x7C, 0x18, 0x3E, 0x7E, 0x00, 0x7E, 0x7F, 0x00, 0xFE, 0x7F, 0x81, 0xFE, 
    //    0x7F, 0x81, 0xFE, 0x7F, 0x00, 0xFE, 0x7E, 0x00, 0x7E, 0x7C, 0x18, 0x3E, 
    //    0x78, 0x3C, 0x1E, 0x70, 0x7E, 0x0E, 0x60, 0xFF, 0x06, 0x61, 0xFF, 0x86, 
    //    0x63, 0xFF, 0xC6, 0x7F, 0xFF, 0xFE, 0x3F, 0xFF, 0xFC, 0x00, 0x00, 0x00,
    //    0                 
    //};

}