#include "import_all.h"
#include <Arduino.h>

#define TEST(TEST, ERR) if (!(TEST)) { Serial.println(ERR); SLEEP(5000); ESP.restart(); }


void setup()
{
    init_all();

    LOGI(e_LOG_TAG::TAG_MAIN, "Main is good.");    
}

void loop() { vTaskDelete(NULL); }