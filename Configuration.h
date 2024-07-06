/*
Configuration.h

# Depends on:
- defaults.h
- SD_card.h

# Description:
- This is the configuration file in code form. Things changed in settings in app should be saved/loaded to/from here.

*/

#pragma once

#include "defaults.h"

// For easier use and no dependency, fixed blocks are used in the config
struct config_fixed_size_data {
    char key[64];
    char value[32];

    config_fixed_size_data();

    void reset();
    void check_and_fix_eof();
};

constexpr size_t config_fixed_size_data_len = sizeof(config_fixed_size_data);

MAKE_SINGLETON_CLASS(MyConfig, {
    // on file, follow order to guarantee all data is saved

    uint64_t m_core_display_screen_saver_steps_time = core_display_screen_saver_steps_time;
    uint64_t m_i2c_packaging_delay = i2c_packaging_delay;

    void load();
public:
    MyConfig();

    void save() const;

    uint64_t get_core_display_screen_saver_steps_time   () const;
    uint64_t get_i2c_packaging_delay                    () const;

    void set_core_display_screen_saver_steps_time       (uint64_t);
    void set_i2c_packaging_delay                        (uint64_t);

});