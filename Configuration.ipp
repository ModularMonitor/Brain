#pragma once

#include "Configuration.h"

#include "SD_card.h"
#include "LOG_ctl.h"

inline config_fixed_size_data::config_fixed_size_data()
{
    reset();
}

inline void config_fixed_size_data::reset()
{
    memset(key, ' ', sizeof(key));
    memset(value, ' ', sizeof(value));
}

inline void config_fixed_size_data::check_and_fix_eof()
{
    for(auto& i : key)   if (i == '\0') i = ' ';
    for(auto& i : value) if (i == '\0') i = ' ';
}

inline void MyConfig::load()
{
    MySDcard& sd = GET(MySDcard);
    if (!sd.is_online()) {
        LOGW_NOSD(e_LOG_TAG::TAG_CFG, "Config cannot be loaded now. SD card is offline.");
        return;
    }

    constexpr char core_display_screen_saver_steps_time_str[] = "core_display_screen_saver_steps_time";
    constexpr size_t core_display_screen_saver_steps_time_l   = sizeof(core_display_screen_saver_steps_time_str) - 1;

    constexpr char i2c_packaging_delay_str[]                  = "i2c_packaging_delay";
    constexpr size_t i2c_packaging_delay_l                    = sizeof(i2c_packaging_delay_str) - 1;

    constexpr char wifi_hotspot_str[]                         = "wifi_hotspot";
    constexpr size_t wifi_hotspot_l                           = sizeof(wifi_hotspot_str) - 1;

    LOGI(e_LOG_TAG::TAG_CFG, "Loading config...");

    for(size_t off = 0;; ++off)
    {
        config_fixed_size_data dat;
        size_t conf = sd.read_from(config_file_path, (char*)&dat, sizeof(dat), sizeof(dat) * off);

        if (conf != sizeof(dat)) break;

        // sad mapping to variables part

        if      (strncmp(dat.key, core_display_screen_saver_steps_time_str, core_display_screen_saver_steps_time_l) == 0)
            set_core_display_screen_saver_steps_time(std::strtoull(dat.value, nullptr, 10));
        else if (strncmp(dat.key, i2c_packaging_delay_str, i2c_packaging_delay_l) == 0)
            set_i2c_packaging_delay(std::strtoull(dat.value, nullptr, 10));
        else if (strncmp(dat.key, wifi_hotspot_str, wifi_hotspot_l) == 0)
            set_wifi_hotspot(std::strtoull(dat.value, nullptr, 10) != 0);
    }

    LOGI(e_LOG_TAG::TAG_CFG, "Config loaded from %s.", config_file_path);
}

inline MyConfig::MyConfig()
{
    load();
}

inline void MyConfig::save() const
{
    MySDcard& sd = GET(MySDcard);

    LOGI(e_LOG_TAG::TAG_CFG, "Config being saved...");

    config_fixed_size_data dat;

    snprintf(dat.key, sizeof(dat.key),      "core_display_screen_saver_steps_time");
    snprintf(dat.value, sizeof(dat.value),  "%llu", m_core_display_screen_saver_steps_time);
    dat.check_and_fix_eof();
    ASSERT_SD_LOCK_OVERWRITE(sd, config_file_path, (char*)&dat, sizeof(dat));

    dat.reset();
    snprintf(dat.key, sizeof(dat.key),      "i2c_packaging_delay");
    snprintf(dat.value, sizeof(dat.value),  "%llu", m_i2c_packaging_delay);
    dat.check_and_fix_eof();
    ASSERT_SD_LOCK_APPEND_ON(sd, config_file_path, (char*)&dat, sizeof(dat));

    dat.reset();
    snprintf(dat.key, sizeof(dat.key),      "wifi_hotspot");
    snprintf(dat.value, sizeof(dat.value),  "%s", m_wifi_hotspot ? "1" : "0");
    dat.check_and_fix_eof();
    ASSERT_SD_LOCK_APPEND_ON(sd, config_file_path, (char*)&dat, sizeof(dat));

    LOGI(e_LOG_TAG::TAG_CFG, "Config saved on %s.", config_file_path);
}

inline uint64_t MyConfig::get_core_display_screen_saver_steps_time() const
{
    return m_core_display_screen_saver_steps_time;
}

inline uint64_t MyConfig::get_i2c_packaging_delay() const
{
    return m_i2c_packaging_delay;
}

inline bool MyConfig::get_wifi_hotspot() const
{
    return m_wifi_hotspot;
}

inline void MyConfig::set_core_display_screen_saver_steps_time(uint64_t v)
{
    if (v >= 5000 || v == 0) { // more than 5 seconds or infinite (0)
        m_core_display_screen_saver_steps_time = v;
        LOGI(e_LOG_TAG::TAG_CFG, "Config changed: core_display_screen_saver_steps_time => %llu", m_core_display_screen_saver_steps_time);
    }
}

inline void MyConfig::set_i2c_packaging_delay(uint64_t v)
{
    if (v >= 5000 && v <= 86400000) {
        m_i2c_packaging_delay = v; // min 5 sec, max 1 day
        LOGI(e_LOG_TAG::TAG_CFG, "Config changed: i2c_packaging_delay => %llu", m_i2c_packaging_delay);
    }
}

inline void MyConfig::set_wifi_hotspot(bool v)
{    
    m_wifi_hotspot = v; // min 1 sec, max 1 day
    LOGI(e_LOG_TAG::TAG_CFG, "Config changed: wifi_hotspot => %s", m_wifi_hotspot ? "ON" : "OFF");
}