#include "import_all.h"
#include <Arduino.h>

#define TEST(TEST, ERR) if (!(TEST)) { Serial.println(ERR); SLEEP(5000); ESP.restart(); }


void setup()
{
    //Serial.begin(logger_serial_speed);
    //while(!Serial.available()) SLEEP(100);
    SLEEP(1000);

    LOGI(e_LOG_TAG::TAG_MAIN, "Main has begun.");
}

void loop() {
    LOGI(e_LOG_TAG::TAG_MAIN, "Main loop");
    SLEEP(1000);
    //vTaskDelete(NULL);
}