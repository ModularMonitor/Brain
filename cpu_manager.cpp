#include "cpu_manager.h"

__i_clk __cpuclk;

uint32_t get_lowest_clock() {    
    switch(getXtalFrequencyMhz()) {
    case RTC_XTAL_FREQ_40M:
        return 10;
    case RTC_XTAL_FREQ_26M:
        return 13;
    case RTC_XTAL_FREQ_24M:
        return 12;
    default:
        return 80;
    }
}

uint32_t get_cpu_clock()
{
    return getCpuFrequencyMhz();
}

uint32_t set_cpu_mode(const cpu_mode mode)
{
    const uint32_t slowest = get_lowest_clock();

    switch (mode)
    {
    case cpu_mode::FAST:
        setCpuFrequencyMhz(240);
        return 240;
    case cpu_mode::MID:
        setCpuFrequencyMhz(80);
        return 80;
    case cpu_mode::SLOW:
        setCpuFrequencyMhz(slowest);
        return slowest;
    }
    return 0;
}


// idle
bool IRAM_ATTR __idl()
{
    ++__cpuclk.m_t_idle;
    return true;
}
// tick
void IRAM_ATTR __tck()
{
    if (++__cpuclk.m_t_tick >= 1000) {
        __cpuclk.m_idle_cpy = __cpuclk.m_t_idle;
        __cpuclk.m_tick_cpy = __cpuclk.m_t_tick;
        __cpuclk.m_t_idle = __cpuclk.m_t_tick = 0;
    }
}

__i_clk::__i_clk()
{    
    esp_register_freertos_idle_hook(__idl);
    esp_register_freertos_tick_hook(__tck);
}



float get_cpu_usage()
{
    const float val = (1.0f - (1.0f * __cpuclk.m_idle_cpy / (__cpuclk.m_tick_cpy + 1)));
    return __cpuclk.m_tick_cpy != 0 ? (val > 1.0f ? 1.0f : (val < 0.0f ? 0.0f : val)) : -1.0f;
}

uint32_t get_cpu_idle_ticks()
{
    return __cpuclk.m_idle_cpy;
}

uint32_t get_cpu_last_ticks()
{
    return __cpuclk.m_tick_cpy;
}

size_t get_ram_free_bytes()
{
    multi_heap_info_t inf;
    heap_caps_get_info(&inf, MALLOC_CAP_8BIT);
    return inf.total_free_bytes;
}

size_t get_ram_total_bytes()
{
    multi_heap_info_t inf;
    heap_caps_get_info(&inf, MALLOC_CAP_8BIT);
    return inf.total_allocated_bytes + inf.total_free_bytes;
}

float get_ram_usage()
{
    multi_heap_info_t inf;
    heap_caps_get_info(&inf, MALLOC_CAP_8BIT);
    const float res = 1.0f * inf.total_allocated_bytes / (1.0f * ((inf.total_allocated_bytes + inf.total_free_bytes) + 1));
    if (res < 0.0f) return 0.0f;
    if (res > 1.0f) return 1.0f;
    return res;
}

unsigned long long get_time_ms()
{
    return std::chrono::duration_cast<std::chrono::duration<unsigned long long, std::milli>>(std::chrono::system_clock::now().time_since_epoch()).count();
}

AutoTiming::AutoTiming(const uint32_t ms)
    : m_now(get_time_ms()), m_last(get_time_ms() + ms)
{
}

AutoTiming::~AutoTiming()
{
    const auto _nw = get_time_ms();
    if (_nw < m_last) vTaskDelay((m_last - _nw) / portTICK_PERIOD_MS);
}