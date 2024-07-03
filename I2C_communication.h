/*
I2C_communication.h

# Depends on:
- defaults.h
- SD_card.h
- Configuration.h
- Serial

# Description:
- This is responsible for the data transmission between devices using I2C.
- This also automatically saves stuff to SD card when things are connected and store information to show on screen later.

*/
#pragma once

#include "defaults.h"

#include "Serial/packaging.h"
#include "Serial/flags.h"

#include <string>
#include <vector>
#include <unordered_map>

using i2c_data_map = std::unordered_map<std::string, char[32]>;

// use GET(MyI2Ccomm) to get its singleton ref!
MAKE_SINGLETON_CLASS(MyI2Ccomm, {
public:
    struct device {
        i2c_data_map m_map;
        uint64_t m_update_time = 0;
        bool m_online = false;
        bool m_issues = false;

        // path, value
        const char* const post_value(const char*, const double&);
        // path, value
        const char* const post_value(const char*, const float&);
        // path, value
        const char* const post_value(const char*, const int64_t&);
        // path, value
        const char* const post_value(const char*, const uint64_t&);
    };

    struct device_history {
        device m_hist[i2c_values_history_size];
        size_t m_hist_point = 0;

        // get current target
        device& get_current_device();
        // advance internal pointer
        void advance_one_device();
    };
private:
    device_history m_devices[CS::d2u(CS::device_id::_MAX)];
    CPU::AutoWait m_sdcard_check_time{i2c_path_checking_time}; // checks paths periodically

    void check_sd_card_paths_existance();

    void async_i2c_caller();
public:
    MyI2Ccomm();
});