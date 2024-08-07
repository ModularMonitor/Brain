#pragma once

#include "defaults.h"
#include "CPU_control.h"


namespace CPU {

    // hacky fancy setup
    namespace _INTERNAL {
        
        struct __ctl {
            volatile uint32_t m_t_idle = 0;
            volatile uint32_t m_t_tick = 0;
            uint32_t m_idle_cpy = 0;
            uint32_t m_tick_cpy = 0;
            bool m_started = false;
        };
        
        inline __ctl& IRAM_ATTR __get_singleton(const uint8_t core) {
            static __ctl ctl[2];
            return core ? ctl[1] : ctl[0];
        }

        // idle
        bool IRAM_ATTR __idl0()
        {
            auto& __cpuclk = __get_singleton(0);
            ++__cpuclk.m_t_idle;
            return true;
        }
        // tick
        void IRAM_ATTR __tck0()
        {
            auto& __cpuclk = __get_singleton(0);
            if (++__cpuclk.m_t_tick >= 1000) {
                __cpuclk.m_idle_cpy = __cpuclk.m_t_idle;
                __cpuclk.m_tick_cpy = __cpuclk.m_t_tick;
                __cpuclk.m_t_idle = __cpuclk.m_t_tick = 0;
            }
        }
        // idle
        bool IRAM_ATTR __idl1()
        {
            auto& __cpuclk = __get_singleton(1);
            ++__cpuclk.m_t_idle;
            return true;
        }
        // tick
        void IRAM_ATTR __tck1()
        {
            auto& __cpuclk = __get_singleton(1);
            if (++__cpuclk.m_t_tick >= 1000) {
                __cpuclk.m_idle_cpy = __cpuclk.m_t_idle;
                __cpuclk.m_tick_cpy = __cpuclk.m_t_tick;
                __cpuclk.m_t_idle = __cpuclk.m_t_tick = 0;
            }
        }

        MAKE_SINGLETON_CLASS_INIT_CF(INITIALIZER,
        {
            auto& ctl0 = __get_singleton(0);
            if (!ctl0.m_started) {
                ctl0.m_started = true;
                esp_register_freertos_idle_hook_for_cpu(__idl0, 0);
                esp_register_freertos_tick_hook_for_cpu(__tck0, 0);
            }

            auto& ctl1 = __get_singleton(1);
            if (!ctl1.m_started) {
                ctl1.m_started = true;
                esp_register_freertos_idle_hook_for_cpu(__idl1, 1);
                esp_register_freertos_tick_hook_for_cpu(__tck1, 1);
            }
            
        }, IRAM_ATTR);
    }
    
    inline AutoWait::AutoWait(const uint64_t ms)
        : m_now(get_time_ms()), m_diff(ms), m_last(get_time_ms() + ms)
    {
    }

    inline AutoWait::~AutoWait()
    {
        const auto _nw = get_time_ms();
        if (_nw < m_last) vTaskDelay((m_last - _nw) / portTICK_PERIOD_MS);
    }

    inline bool AutoWait::is_time()
    {
        const auto _nw = get_time_ms();
        if (_nw >= m_last) {
            m_last = _nw + m_diff;
            return true;
        }
        return false;
    }

    inline uint32_t get_lowest_clock_mhz() {    
        switch(getXtalFrequencyMhz()) {
        case RTC_XTAL_FREQ_40M: return 10;
        case RTC_XTAL_FREQ_26M: return 13;
        case RTC_XTAL_FREQ_24M: return 12;
        default: return 80;
        }
    }
    
    inline uint32_t get_highest_clock_mhz()
    {
        return 240; // fixed
    }

    inline uint32_t get_cpu_clock_mhz()
    {
        return getCpuFrequencyMhz();
    }

    inline void set_clock_mhz(const uint32_t clk)
    {
        const auto _min = get_lowest_clock_mhz();
        const auto _max = get_highest_clock_mhz();
        const uint32_t nclk = clk < _min ? _min : (clk > _max ? _max : clk);
        LOGI(e_LOG_TAG::TAG_CPU, "Call registered to set CPU to %u MHz.", nclk);
        setCpuFrequencyMhz(nclk);
    }
    
    inline float get_cpu_usage()
    {
        return (get_cpu_usage(0) + get_cpu_usage(1)) * 0.5f;
    }

    inline float get_cpu_usage(uint8_t core)
    {
        if (core > 1) core = 1;

        const auto& __cpuclk = _INTERNAL::__get_singleton(core);

        const float val = (1.0f - (1.0f * __cpuclk.m_idle_cpy / (__cpuclk.m_tick_cpy + 1)));
        return __cpuclk.m_tick_cpy != 0 ? (val > 1.0f ? 1.0f : (val < 0.0f ? 0.0f : val)) : -1.0f;
    }
    
    inline float get_ram_usage()
    {
        multi_heap_info_t inf;
        heap_caps_get_info(&inf, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT);
        const float res = 1.0f * inf.total_allocated_bytes / (1.0f * ((inf.total_allocated_bytes + inf.total_free_bytes) + 1));
        if (res < 0.0f) return 0.0f;
        if (res > 1.0f) return 1.0f;
        return res;
    }

    //inline uint32_t get_random()
    //{
    //    return esp_random();
    //}
    //
    //inline uint8_t get_core_id()
    //{
    //    return xPortGetCoreID();
    //}
}