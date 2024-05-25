#pragma once

#include "common.h"

MAKE_SINGLETON_CLASS_INIT_C(SETUPSELF, 
    Serial.begin(115200);
    Serial.printf("Started MASTER\n");
);
