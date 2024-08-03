#pragma once

/* Headers */
#include "defaults.h"
#include "CPU_control.h"
#include "SD_card.h"
#include "Configuration.h"
#include "LOG_ctl.h"
#include "I2C_communication.h"
#include "CORE_Display_aux_draw.h"
#include "CORE_Display.h"
#include "Serial_input_handler.h"

#include "wifi_server.h"

/* Implementations */
#include "CPU_control.ipp"
#include "SD_card.ipp"
#include "Configuration.ipp"
#include "LOG_ctl.ipp"
#include "I2C_communication.ipp"
#include "CORE_Display_aux_draw.ipp"
#include "CORE_Display.ipp"
#include "Serial_input_handler.ipp"

inline void init_all() {
    // CPU does by itself
    const auto& sd = GET(MySDcard);
    const auto& log = GET(MyLOG);

    LOGI(e_LOG_TAG::TAG_MAIN, "==============================");
    LOGI(e_LOG_TAG::TAG_MAIN, "| Core initialization begun! |");
    LOGI(e_LOG_TAG::TAG_MAIN, "==============================");
    LOGI(e_LOG_TAG::TAG_MAIN, "System version: %s", app_version);
    LOGI(e_LOG_TAG::TAG_MAIN, "Developed by: Lohk & Enigma");
    LOGI(e_LOG_TAG::TAG_MAIN, "2024 TCC Project \"Modular Monitor\"");
    LOGI(e_LOG_TAG::TAG_MAIN, "");
    LOGI(e_LOG_TAG::TAG_MAIN, "Welcome aboard, user. Let's begin.");
    LOGI(e_LOG_TAG::TAG_MAIN, "");

    const auto& cfg = GET(MyConfig);
    const auto& i2c = GET(MyI2Ccomm);
    const auto& core = GET(CoreDisplay);
    const auto& serial = GET(MySerialReader);
}