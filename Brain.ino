#include "import_all.h"
#include <Arduino.h>

#define TEST(TEST, ERR) if (!(TEST)) { Serial.println(ERR); SLEEP(5000); ESP.restart(); }


void setup()
{
    //Serial.begin(logger_serial_speed);
    //while(!Serial.available()) SLEEP(100);
    SLEEP(1000);
    init_all();

    LOGI(e_LOG_TAG::TAG_MAIN, "Main has begun.");
}

void loop() {
    LOGI(e_LOG_TAG::TAG_MAIN, "Main loop");
    LOGI(e_LOG_TAG::TAG_MAIN, "Performance stats: %05.2f%% (%05.2f%% | %05.2f%%)", CPU::get_cpu_usage(), CPU::get_cpu_usage(0), CPU::get_cpu_usage(1));

    SLEEP(1000);
    //vTaskDelete(NULL);
}