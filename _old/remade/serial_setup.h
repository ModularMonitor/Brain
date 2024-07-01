#pragma once

#include "common.h"

MAKE_SINGLETON_CLASS_INIT_C(SETUPSELF, 
    Serial.begin(115200);
    Serial.printf("Started MASTER\n");

    //if (!digitalRead(0)) {
    //    Serial.printf("ON HOLD BY BUTTON!");
    //
    //    while (1) {
    //        yield();
    //        if (Serial.available()) break;
    //    }
    //}
);
