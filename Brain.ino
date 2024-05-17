//#define _DEBUG

#include "i2c_driver.h"
#include "display_class.h"
#include "cpu_manager.h"
#include "sdcard.h"

/*
References of pins: [Setup21_ILI9488]
#define TFT_MISO 19 // (leave TFT SDO disconnected if other SPI devices share MISO)
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)
#define TOUCH_CS 16
#define SD_SPI_MOSI	23
#define SD_SPI_MISO	19
#define SD_SPI_SCLK	18
#define SD_SPI_SEL 5
*/

using namespace CS;

const int cpu_display_id = 0; // 1 is used by Arduino by default, let's use 0
const int cpu_thinking_id = 1; // default Arduino thread
const UBaseType_t display_pri = 1;
const UBaseType_t thinking_pri = 0;

void setup()
{
    Serial.begin(115200);
    //while(!Serial);
    //delay(2000);
    
    Serial.printf("Starting MASTER\n");

    if (!SDcard::sd_init()) {
        Serial.printf("No SD card available\n");
    }
    else { // prepare for all

    }

    auto all = SDcard::list_dir("/");

    for(auto& i : all) {
        Serial.printf("%s => %zu\n", i.name.c_str(), i.size);
    }

    xTaskCreatePinnedToCore(loop_display, "DISPLAYTHR", 8192, nullptr, display_pri, nullptr, cpu_display_id);
    xTaskCreatePinnedToCore(loop_think, "THINKTHR", 8192, nullptr, thinking_pri, nullptr, cpu_thinking_id);
}

void loop() { vTaskDelete(NULL); }