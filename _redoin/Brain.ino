//#define _DEBUG

#include "defaults.h"
#include "i2c_driver.h"
#include "display_class.h"
#include "cpu_manager.h"
#include "sdcard.h"


using namespace CS;


void setup()
{
    Serial.begin(115200);
    //while(!Serial);
    //delay(2000);
    
    Serial.printf("Starting MASTER\n");

    //if (!SDcard::sd_init()) {
    //    Serial.printf("No SD card available\n");
    //}
    //else { // prepare for all
//
    //}
//
    //auto all = SDcard::list_dir("/");
//
    //for(auto& i : all) {
    //    Serial.printf("%s => %zu\n", i.name.c_str(), i.size);
    //}

    xTaskCreatePinnedToCore(loop_display, "DISPLAYTHR", 8192, nullptr, display_pri, nullptr, cpu_display_id);
    xTaskCreatePinnedToCore(loop_think, "THINKTHR", 8192, nullptr, thinking_pri, nullptr, cpu_thinking_id);
}

void loop() { 
    //vTaskDelete(NULL);

    if (Serial.available()) {
        const auto str = Serial.readStringUntil('\n');

        Serial.print("Got: ");
        Serial.println(str);

        if (str == "sd_list") {            
            auto all = SDcard::list_dir("/");
            for(auto& i : all) {
                Serial.printf("%s => %zu\n", i.name.c_str(), i.size);
            }
        }
    }
    else delay(100);
}