#pragma once

#include "I2C_communication.h"
#include "CPU_control.h"
#include "SD_card.h"
#include "Configuration.h"

#include <memory>


inline const char* const MyI2Ccomm::device::post_value(const char* path, const double& v)
{
    auto& item = m_map[path];
    snprintf(item, sizeof(item), "%.8lf", v);
    m_update_time = get_time_ms();
    return item;
}

inline const char* const MyI2Ccomm::device::post_value(const char* path, const float& v)
{
    auto& item = m_map[path];
    snprintf(item, sizeof(item), "%.6f", v);
    m_update_time = get_time_ms();
    return item;
}

inline const char* const MyI2Ccomm::device::post_value(const char* path, const int64_t& v)
{
    auto& item = m_map[path];
    snprintf(item, sizeof(item), "%" PRId64, v);
    m_update_time = get_time_ms();
    return item;
}

inline const char* const MyI2Ccomm::device::post_value(const char* path, const uint64_t& v)
{
    auto& item = m_map[path];
    snprintf(item, sizeof(item), "%" PRIu64, v);
    m_update_time = get_time_ms();
    return item;
}

inline MyI2Ccomm::device& MyI2Ccomm::device_history::get_current_device()
{
    return m_hist[m_hist_point];
}

inline void MyI2Ccomm::device_history::advance_one_device()
{
    m_hist_point = ((m_hist_point + 1) % i2c_values_history_size);
}

inline void MyI2Ccomm::check_sd_card_paths_existance()
{
    MySDcard& sd = GET(MySDcard);

    if (!sd.is_online()) return;

    if (sd.dir_exists("/i2c") || sd.make_dir("/i2c")) {
        //LOGI(e_LOG_TAG::TAG_I2C, "Preparing SD card ground for devices...");
        for(uint8_t p = 0; p < CS::d2u(CS::device_id::_MAX); ++p) {
            char buf[80];
            snprintf(buf, 80, "/i2c/%s", CS::d2str(static_cast<CS::device_id>(p)));
            if (!sd.dir_exists(buf)) sd.make_dir(buf);
        }
        //LOGI(e_LOG_TAG::TAG_I2C, "Paths are ready!");
    }
    else {
        //LOGW(e_LOG_TAG::TAG_I2C, "SD card not present. Not saving data.");
    }
}

inline void MyI2Ccomm::async_i2c_caller()
{
    LOGI(e_LOG_TAG::TAG_I2C, "Setting up wire connection...");

    MySDcard& sd = GET(MySDcard);
    auto wire = std::unique_ptr<CS::PackagedWired>(new CS::PackagedWired(CS::config().set_master().set_sda(i2c_pins[0]).set_scl(i2c_pins[1])));
    char tempbuf[128];

    LOGI(e_LOG_TAG::TAG_I2C, "Making dirs...");

    check_sd_card_paths_existance();

    LOGI(e_LOG_TAG::TAG_I2C, "Initialized.");

    const auto cleanup_to_tempbuf = [&](const char* path) -> char* {
        snprintf(tempbuf, sizeof(tempbuf), "%s", path);
        for(auto& i : tempbuf) {
            if (i == '\0') break;
            if (i == '/' || i == '\\') i = '_';
        }
        return tempbuf;
    };

    while(1) {
        CPU::AutoWait autotime(GET(MyConfig).get_i2c_packaging_delay()); // loop control

        if (m_sdcard_check_time.is_time()) {
            check_sd_card_paths_existance();
        }

        for(uint8_t p = 0; p < CS::d2u(CS::device_id::_MAX); ++p)
        {
            const auto curr = static_cast<CS::device_id>(p);
            
            device& dev = m_devices[p].get_current_device();
            
            CS::FlagWrapper fw;
            auto lst = wire->master_smart_request_all(curr, fw, dev.m_online);
            dev.m_issues = fw & CS::device_flags::HAS_ISSUES;

            if (!dev.m_online || dev.m_issues) continue;

            // advance only if online and with no issues!
            m_devices[p].advance_one_device();

            char device_file_path[128]{}; // used for path on sd card

            for(const auto& i : lst) {
                switch(i.get_type()) {
                case CS::Command::vtype::TD:
                {
                    const char* const data_written_ref = dev.post_value(i.get_path(), i.get_val<double>());

                    snprintf(device_file_path, sizeof(device_file_path), "/i2c/%s/%s.log", CS::d2str(curr), cleanup_to_tempbuf(i.get_path()));

                    const int to_write_on_sd_card = snprintf(tempbuf, sizeof(tempbuf), "%llu,%s\n", get_time_ms(), data_written_ref);
                    if (to_write_on_sd_card > 0) sd.append_on(device_file_path, tempbuf, to_write_on_sd_card);

                    LOGI(e_LOG_TAG::TAG_I2C, "%s => %s", i.get_path(), data_written_ref);
                }
                    break;
                case CS::Command::vtype::TF:
                {
                    const char* const data_written_ref = dev.post_value(i.get_path(), i.get_val<float>());

                    snprintf(device_file_path, sizeof(device_file_path), "/i2c/%s/%s.log", CS::d2str(curr), cleanup_to_tempbuf(i.get_path()));

                    const int to_write_on_sd_card = snprintf(tempbuf, sizeof(tempbuf), "%llu,%s\n", get_time_ms(), data_written_ref);
                    if (to_write_on_sd_card > 0) sd.append_on(device_file_path, tempbuf, to_write_on_sd_card);

                    LOGI(e_LOG_TAG::TAG_I2C, "%s => %s", i.get_path(), data_written_ref);
                }
                    break;
                case CS::Command::vtype::TI:
                {
                    const char* const data_written_ref = dev.post_value(i.get_path(), i.get_val<int64_t>());

                    snprintf(device_file_path, sizeof(device_file_path), "/i2c/%s/%s.log", CS::d2str(curr), cleanup_to_tempbuf(i.get_path()));

                    const int to_write_on_sd_card = snprintf(tempbuf, sizeof(tempbuf), "%llu,%s\n", get_time_ms(), data_written_ref);
                    if (to_write_on_sd_card > 0) sd.append_on(device_file_path, tempbuf, to_write_on_sd_card);

                    LOGI(e_LOG_TAG::TAG_I2C, "%s => %s", i.get_path(), data_written_ref);
                }
                    break;
                case CS::Command::vtype::TU:
                {
                    const char* const data_written_ref = dev.post_value(i.get_path(), i.get_val<uint64_t>());

                    snprintf(device_file_path, sizeof(device_file_path), "/i2c/%s/%s.log", CS::d2str(curr), cleanup_to_tempbuf(i.get_path()));

                    const int to_write_on_sd_card = snprintf(tempbuf, sizeof(tempbuf), "%llu,%s\n", get_time_ms(), data_written_ref);
                    if (to_write_on_sd_card > 0) sd.append_on(device_file_path, tempbuf, to_write_on_sd_card);

                    LOGI(e_LOG_TAG::TAG_I2C, "%s => %s", i.get_path(), data_written_ref);
                }
                    break;
                default:
                    break;
                }
            }
        }
    }

    vTaskDelete(NULL);
}

inline MyI2Ccomm::MyI2Ccomm()
{
    async_class_method_pri(MyI2Ccomm, async_i2c_caller, i2c_thread_priority, cpu_core_id_for_i2c);
}