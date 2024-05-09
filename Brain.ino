//#define _DEBUG

#include "display_driver.h"
#include "i2c_driver.h"
#include "display_class.h"

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

    xTaskCreatePinnedToCore(loop_display, "DISPLAYTHR", 8192, nullptr, display_pri, nullptr, cpu_display_id);
    xTaskCreatePinnedToCore(loop_think, "THINKTHR", 8192, nullptr, thinking_pri, nullptr, cpu_thinking_id);
}

void loop() { vTaskDelete(NULL); }