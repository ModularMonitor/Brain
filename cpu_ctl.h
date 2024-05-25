#pragma once

#include "esp32/clk.h"
#include "esp32-hal.h"
#include "esp32-hal-cpu.h"
#include "soc/rtc.h"
#include "esp_freertos_hooks.h"

#include <chrono>

namespace CPU {

    static const char TAG[] = "CPU";

// auto create task auto
#define acta(FUNCNAME) { CPU::create_task(FUNCNAME, #FUNCNAME); }
// auto create task priority
#define actp(FUNCNAME, PRIORITY) { CPU::create_task(FUNCNAME, #FUNCNAME, PRIORITY); }
// auto create task core
#define actc(FUNCNAME, COREID) { CPU::create_task(FUNCNAME, #FUNCNAME, tskIDLE_PRIORITY, cpu_stack_default, nullptr, COREID); }
// auto create task core prio
#define actcp(FUNCNAME, COREID, PRIORITY) { CPU::create_task(FUNCNAME, #FUNCNAME, PRIORITY, cpu_stack_default, nullptr, COREID); }
// auto create task core prio body
#define actcpb(BODY, COREID, PRIORITY) { CPU::create_task([](void*){ BODY; exit_task(); }, "ASYNCAUTO", PRIORITY, cpu_stack_default, nullptr, COREID); }
// auto create task core prio body argument
#define actcpba(BODY, COREID, PRIORITY, ARG) { CPU::create_task([](void* arg){ BODY; exit_task(); }, "ASYNCAUTO", PRIORITY, cpu_stack_default, ARG, COREID); }

#define exit_task() vTaskDelete(NULL)
#define sleep_for(MILLISEC) { yield(); vTaskDelay(MILLISEC / portTICK_PERIOD_MS); }

    class AutoWait {
        const uint64_t m_now, m_diff;
        uint64_t m_last;
    public:
        AutoWait(const uint64_t ms);
        ~AutoWait();

        bool is_time();
    };
    
    uint32_t get_lowest_clock_mhz();
    uint32_t get_highest_clock_mhz();

    void set_clock_mhz(const uint32_t);

    float get_cpu_usage();
    float get_cpu_usage(const uint8_t);
    float get_ram_usage();

    uint64_t get_time_ms();
    uint32_t get_random();
    uint8_t get_core_id();
    uint32_t get_cpu_clock_mhz();

    TaskHandle_t create_task(void(*f)(void*), const char* nam, UBaseType_t priority = tskIDLE_PRIORITY, size_t stac = cpu_stack_default, void* arg = nullptr, int coreid = -1);

    bool run_on_core_sync(void(*function)(void*), UBaseType_t core_id, void* arg = nullptr);
}

#include "cpu_ctl.ipp"