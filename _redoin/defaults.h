#pragma once

#include <stdint.h>

constexpr int def_spi_core_id = 1;
constexpr int def_alt_core_id = 0;

constexpr int cpu_display_id = def_spi_core_id;
constexpr int cpu_sd_card_id = def_spi_core_id;

constexpr int cpu_thinking_id = def_alt_core_id;

const uint8_t display_pri = 1;
const uint8_t thinking_pri = 0;

/* == NOTE: THESE ARE READ ONLY. SETUP IS ELSEWHERE! == */
/* TFT: Setup21_ILI9488.h */
/* SD: defaults for Arduino */

// /* shared SPI info */
// constexpr int pin_spi_miso = 19;
// constexpr int pin_spi_mosi = 23;
// constexpr int pin_spi_clk = 18;
// 
// /* tft related pins */
// constexpr int pin_tft_cs = 15;
// constexpr int pin_tft_data_cmd = 2;
// constexpr int pin_tft_reset = 4;
// constexpr int pin_tft_touch_cs = 16;
// 
// /* sd card pins */
// constexpr int pin_sd_select = 5;