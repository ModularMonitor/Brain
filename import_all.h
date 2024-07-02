#pragma once

/* Headers */
#include "defaults.h"
#include "CPU_control.h"
#include "SD_card.h"
#include "LOG_ctl.h"
#include "I2C_communication.h"
#include "CORE_Display.h"

/* Implementations */
#include "CPU_control.ipp"
#include "SD_card.ipp"
#include "LOG_ctl.ipp"
#include "I2C_communication.ipp"
#include "CORE_Display.ipp"

inline void init_all() {
    // CPU does by itself
    const auto& sd = GET(MySDcard);
    const auto& log = GET(MyLOG);
    const auto& i2c = GET(MyI2Ccomm);
    const auto& core = GET(CoreDisplay);
}