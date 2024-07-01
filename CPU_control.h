#pragma once

#include "esp32-hal.h"
#include "esp32-hal-cpu.h"
#include "soc/rtc.h"
#include "esp_freertos_hooks.h"

#include <chrono>

namespace CPU {

    class AutoWait {
        const uint64_t m_now, m_diff;
        uint64_t m_last;
    public:
        AutoWait(const uint64_t ms);
        ~AutoWait();

        // Returns true if time set was past. Auto-resets for more of that time, perfectly timed.
        bool is_time();
    };
    
    uint32_t get_lowest_clock_mhz();
    uint32_t get_highest_clock_mhz();
    uint32_t get_cpu_clock_mhz();
    void set_clock_mhz(const uint32_t);

    float get_cpu_usage();
    float get_cpu_usage(const uint8_t);
    float get_ram_usage();

    //uint32_t get_random();
    //uint8_t get_core_id();
}