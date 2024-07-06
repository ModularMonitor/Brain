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
#include <map>
#include <mutex>
#include <optional>

using i2c_data_map = std::map<std::string, char[32]>;
using i2c_data_pair = std::pair<const std::string, char[32]>;


// use GET(MyI2Ccomm) to get its singleton ref!
MAKE_SINGLETON_CLASS(MyI2Ccomm, {
public:
    struct device {
        i2c_data_map m_map;
        mutable std::mutex m_map_mtx;
        uint64_t m_update_time = 0;

        // path, value
        const char* const post_value_nolock(const char*, const double&);
        // path, value
        const char* const post_value_nolock(const char*, const float&);
        // path, value
        const char* const post_value_nolock(const char*, const int64_t&);
        // path, value
        const char* const post_value_nolock(const char*, const uint64_t&);
    };

    struct device_history {
        device m_hist[i2c_values_history_size];
        size_t m_hist_point = 0;

        bool m_online = false;
        bool m_issues = false;

        // get current target
        device& get_current_device();
        // advance internal pointer
        void advance_one_device();

        // for read only, shows last or before last information
        const device& get_in_time(const size_t) const;
    };
private:
    device_history m_devices[CS::d2u(CS::device_id::_MAX)];
    CPU::AutoWait m_sdcard_check_time{i2c_path_checking_time}; // checks paths periodically

    void check_sd_card_paths_existance();

    void async_i2c_caller();
public:
    MyI2Ccomm();

    // basically returns i2c_values_history_size
    static constexpr size_t get_max_history_size() {return i2c_values_history_size;}

    // Get if device was online last time it checked its data
    bool is_device_online(CS::device_id) const;

    // Get if device had issues last time it checked its data
    bool is_device_with_issue(CS::device_id) const;

    // Goes to m_devices[dev][back_in...] and returns the m_map length.
    const device& get_device_configurations(const CS::device_id dev, const size_t back_in_time_idx) const;

    // Get a pair of key and value in this device information in time based on index (may change order each run).
    const i2c_data_pair& get_device_data_in_time(const CS::device_id dev, const size_t back_in_time_idx, const size_t map_idx) const;

    // Get a pair of key and value in this device information in time based on key value.
    //const i2c_data_pair& get_device_data_in_time(const CS::device_id dev, const size_t back_in_time_idx, const std::string& map_key) const;
});


const char* get_fancy_name_for(CS::device_id id);