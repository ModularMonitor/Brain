#pragma once

#include "I2C_communication.h"
#include "CPU_control.h"
#include "SD_card.h"
#include "Configuration.h"

#include <memory>


inline const char* const MyI2Ccomm::device::post_value_nolock(const char* path, const double& v)
{
    auto& item = m_map[path];
    snprintf(item, sizeof(item), "%.8lf", v);
    m_update_time = get_time_ms();
    return item;
}

inline const char* const MyI2Ccomm::device::post_value_nolock(const char* path, const float& v)
{
    auto& item = m_map[path];
    snprintf(item, sizeof(item), "%.6f", v);
    m_update_time = get_time_ms();
    return item;
}

inline const char* const MyI2Ccomm::device::post_value_nolock(const char* path, const int64_t& v)
{
    auto& item = m_map[path];
    snprintf(item, sizeof(item), "%" PRId64, v);
    m_update_time = get_time_ms();
    return item;
}

inline const char* const MyI2Ccomm::device::post_value_nolock(const char* path, const uint64_t& v)
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
    //Serial.printf("IDCIDCIDC %zu\n", m_hist_point);
}

inline const MyI2Ccomm::device& MyI2Ccomm::device_history::get_in_time(const size_t idx) const
{
    const size_t real_p = (m_hist_point + idx + i2c_values_history_size - 1) % i2c_values_history_size; // guaranteed limit until the infinites ;P
    return m_hist[real_p];
}

inline void MyI2Ccomm::check_sd_card_paths_existance()
{
    MySDcard& sd = GET(MySDcard);

    if (!sd.is_online()) return;

    if (sd.dir_exists("/i2c") || sd.make_dir("/i2c")) {
        for(uint8_t p = 0; p < CS::d2u(CS::device_id::_MAX); ++p) {
            char buf[80];
            snprintf(buf, 80, "/i2c/%s", CS::d2str(static_cast<CS::device_id>(p)));
            if (!sd.dir_exists(buf)) sd.make_dir(buf);
        }
    }
}

inline void MyI2Ccomm::async_i2c_caller()
{
    LOGI(e_LOG_TAG::TAG_I2C, "Setting up wire connection...");

    MySDcard& sd = GET(MySDcard);
    auto wire = std::unique_ptr<CS::PackagedWired>(new CS::PackagedWired(CS::config().set_master().set_sda(i2c_pins[0]).set_scl(i2c_pins[1])));
    char tempbuf[128];
    uint64_t last_run = 0;

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
        while(last_run + GET(MyConfig).get_i2c_packaging_delay() > get_time_ms()) SLEEP(50);
        last_run = get_time_ms();

        //CPU::AutoWait autotime(GET(MyConfig).get_i2c_packaging_delay()); // loop control

        if (m_sdcard_check_time.is_time()) {
            check_sd_card_paths_existance();
        }

        for(uint8_t p = 0; p < CS::d2u(CS::device_id::_MAX); ++p)
        {
            const auto curr = static_cast<CS::device_id>(p);
            
            device& dev = m_devices[p].get_current_device();
            
            CS::FlagWrapper fw;
            auto lst = wire->master_smart_request_all(curr, fw, m_devices[p].m_online);
            m_devices[p].m_issues = fw & CS::device_flags::HAS_ISSUES;

            if (!m_devices[p].m_online || m_devices[p].m_issues) continue;
            if (lst.empty()) continue; // ignore if no data to append!

            // advance only if online and with no issues!
            m_devices[p].advance_one_device();

            char device_file_path[128]{}; // used for path on sd card

            std::lock_guard<std::mutex> l(dev.m_map_mtx); // secure

            for(const auto& i : lst) {
                switch(i.get_type()) {
                case CS::Command::vtype::TD:
                {
                    const char* const data_written_ref = dev.post_value_nolock(i.get_path(), i.get_val<double>());

                    snprintf(device_file_path, sizeof(device_file_path), "/i2c/%s/%s.log", CS::d2str(curr), cleanup_to_tempbuf(i.get_path()));

                    const int to_write_on_sd_card = snprintf(tempbuf, sizeof(tempbuf), "%llu,%s\n", get_time_ms(), data_written_ref);
                    if (to_write_on_sd_card > 0) sd.append_on(device_file_path, tempbuf, to_write_on_sd_card);

                    //LOGI(e_LOG_TAG::TAG_I2C, "%s => %s", i.get_path(), data_written_ref);
                }
                    break;
                case CS::Command::vtype::TF:
                {
                    const char* const data_written_ref = dev.post_value_nolock(i.get_path(), i.get_val<float>());

                    snprintf(device_file_path, sizeof(device_file_path), "/i2c/%s/%s.log", CS::d2str(curr), cleanup_to_tempbuf(i.get_path()));

                    const int to_write_on_sd_card = snprintf(tempbuf, sizeof(tempbuf), "%llu,%s\n", get_time_ms(), data_written_ref);
                    if (to_write_on_sd_card > 0) sd.append_on(device_file_path, tempbuf, to_write_on_sd_card);

                    //LOGI(e_LOG_TAG::TAG_I2C, "%s => %s", i.get_path(), data_written_ref);
                }
                    break;
                case CS::Command::vtype::TI:
                {
                    const char* const data_written_ref = dev.post_value_nolock(i.get_path(), i.get_val<int64_t>());

                    snprintf(device_file_path, sizeof(device_file_path), "/i2c/%s/%s.log", CS::d2str(curr), cleanup_to_tempbuf(i.get_path()));

                    const int to_write_on_sd_card = snprintf(tempbuf, sizeof(tempbuf), "%llu,%s\n", get_time_ms(), data_written_ref);
                    if (to_write_on_sd_card > 0) sd.append_on(device_file_path, tempbuf, to_write_on_sd_card);

                    //LOGI(e_LOG_TAG::TAG_I2C, "%s => %s", i.get_path(), data_written_ref);
                }
                    break;
                case CS::Command::vtype::TU:
                {
                    const char* const data_written_ref = dev.post_value_nolock(i.get_path(), i.get_val<uint64_t>());

                    snprintf(device_file_path, sizeof(device_file_path), "/i2c/%s/%s.log", CS::d2str(curr), cleanup_to_tempbuf(i.get_path()));

                    const int to_write_on_sd_card = snprintf(tempbuf, sizeof(tempbuf), "%llu,%s\n", get_time_ms(), data_written_ref);
                    if (to_write_on_sd_card > 0) sd.append_on(device_file_path, tempbuf, to_write_on_sd_card);

                    //LOGI(e_LOG_TAG::TAG_I2C, "%s => %s", i.get_path(), data_written_ref);
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

inline bool MyI2Ccomm::is_device_online(CS::device_id dev) const
{
    return m_devices[CS::d2u(dev)].m_online;
}

inline bool MyI2Ccomm::is_device_with_issue(CS::device_id dev) const
{
    return m_devices[CS::d2u(dev)].m_issues;
}

// Goes to m_devices[dev][back_in...] and returns the m_map length.
inline const MyI2Ccomm::device& MyI2Ccomm::get_device_configurations(const CS::device_id dev, const size_t back_in_time_idx) const
{
    return m_devices[CS::d2u(dev)].get_in_time(back_in_time_idx);
}

inline const i2c_data_pair& MyI2Ccomm::get_device_data_in_time(const CS::device_id dev, const size_t back_in_time_idx, const size_t map_idx) const
{
    static i2c_data_pair nul;
    const auto& ref = m_devices[CS::d2u(dev)].get_in_time(back_in_time_idx);
    std::lock_guard<std::mutex> l(ref.m_map_mtx);
    return map_idx < ref.m_map.size() ? *std::next(ref.m_map.begin(), map_idx) : nul;
}

//inline const i2c_data_pair& MyI2Ccomm::get_device_data_in_time(const CS::device_id dev, const size_t back_in_time_idx, const std::string& map_key) const
//{
//    static i2c_data_pair nul;
//
//    const auto& ref = m_devices[CS::d2u(dev)].get_in_time(back_in_time_idx);
//    std::lock_guard<std::mutex> l(ref.m_map_mtx);
//
//    const auto& mmap = ref.m_map;
//
//    auto it = mmap.find(map_key);
//
//    return it != mmap.end() ? *it : nul;
//}



inline const char* get_fancy_name_for(CS::device_id id)
{
    switch(id) {
    case CS::device_id::DHT22_SENSOR:       return "Modulo de temperatura e umidade";
    case CS::device_id::MICS_6814_SENSOR:   return "Modulo de NO2, NH3 e CO";
    case CS::device_id::LY038_HW072_SENSOR: return "Modulo de som e luz";
    case CS::device_id::GY87_SENSOR:        return "Modulo acelerometro, giroscopio, barometro";
    case CS::device_id::CCS811_SENSOR:      return "Modulo de eCO2 e TVOC";
    case CS::device_id::PMSDS011_SENSOR:    return "Modulo de particulas no ar";
    case CS::device_id::BATTERY_SENSOR:     return "Modulo da bateria";
    default:                                return "Desconhecido";
    }
}