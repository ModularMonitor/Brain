#pragma once

#include "esp32/clk.h"
#include "esp32-hal.h"
#include "esp32-hal-cpu.h"
#include "soc/rtc.h"
#include "esp_freertos_hooks.h"

#include <chrono>

// 240 Mhz, 80 Mhz, lowest MHz
enum class cpu_mode { FAST, MID, SLOW };

uint32_t get_lowest_clock();

uint32_t get_cpu_clock();

uint32_t set_cpu_mode(const cpu_mode);

// CPU counters //

struct __i_clk {
    volatile uint32_t m_t_idle = 0;
    volatile uint32_t m_t_tick = 0;
    uint32_t m_idle_cpy = 0;
    uint32_t m_tick_cpy = 0;

    __i_clk();
};

extern __i_clk __cpuclk;


float get_cpu_usage();
uint32_t get_cpu_idle_ticks();
uint32_t get_cpu_last_ticks();
size_t get_ram_free_bytes();
size_t get_ram_total_bytes();
float get_ram_usage();

uint64_t get_time_ms();

class AutoTiming {
    const uint64_t m_now, m_last;
public:
    AutoTiming(const uint32_t ms);
    ~AutoTiming();
};

class TimingLoop {
    const uint32_t m_diff;
    uint64_t m_next;
public:
    TimingLoop(const uint32_t ms);
    
    bool is_time();
};