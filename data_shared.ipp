#pragma once

#include "data_shared.h"

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

namespace STR {
   
//    template<typename T>
//    inline size_t _to_hex_format(char* targ_or_calc_null, const T& v)
//    {
//        uint8_t* p = (uint8_t*)&v;
//        const size_t len = sizeof(T);
//        if (!targ_or_calc_null) return (len * 3); // \0
//        
//        for(size_t i = 0; i < len; ++i) {
//            sprintf(targ_or_calc_null + (i * 3), (i + 1 == len) ? "%02X" : "%02X|", p[i]);
//        }
//        
//        return len * 3 + 1;
//    }
//
//    template<typename T>
//    inline bool _from_hex_format(const char* str, T& targ)
//    {
//        uint8_t* p = (uint8_t*)&targ;
//        const size_t len = sizeof(T);
//        const size_t slen = strlen(str);
//
//        if ((slen + 1) / 3 < len) return false;
//        
//        for(size_t i = 0; i < len; ++i) {
//            p[i] = std::strtol(str + (i * 3), nullptr, 16);
//        }
//        return true;
//    }




    template<typename T>
    inline RepresentedData::data_storage::data_storage(const T& v)
    {
        set_value(v);
    }

    inline void RepresentedData::data_storage::set_value(const double& v)
    {
        snprintf(m_value_str, sizeof(m_value_str), "%.8lf", v);
        m_update_time = CPU::get_time_ms();
    }

    inline void RepresentedData::data_storage::set_value(const float& v)
    {
        snprintf(m_value_str, sizeof(m_value_str), "%.6f", v);
        m_update_time = CPU::get_time_ms();
    }

    inline void RepresentedData::data_storage::set_value(const int64_t& v)
    {
        snprintf(m_value_str, sizeof(m_value_str), "%" PRId64, v);
        m_update_time = CPU::get_time_ms();
    }

    inline void RepresentedData::data_storage::set_value(const uint64_t& v)
    {
        snprintf(m_value_str, sizeof(m_value_str), "%" PRIu64, v);
        m_update_time = CPU::get_time_ms();
    }

    inline const char* RepresentedData::data_storage::get_value() const
    {
        return m_value_str;
    }

    inline uint64_t RepresentedData::data_storage::get_modified_ms() const
    {
        return m_update_time;
    }


    inline void RepresentedData::advance_ptr()
    {
        if (++m_current_data >= (m_data + max_data_stored_array)) m_current_data = m_data;
    }

    template<typename T>
    inline RepresentedData::RepresentedData(const char* s, const T& v)
    {
        size_t len = strlen(s);
        if (len <= 0) return;
        if (len > CS::max_path_len) len = CS::max_path_len;
        memcpy(m_path, s, len);

        set_value(v);
    }

    template<typename T>
    inline void RepresentedData::set_value(const T& v)
    {
        m_current_data->set_value(v);
        advance_ptr();
    }

    inline bool RepresentedData::is_path(const char* s) const
    {
        size_t len = strlen(s);
        size_t mlen = strlen(m_path);
        if (len <= 0 || len > CS::max_path_len || mlen != len) return false;
        return memcmp(m_path, s, mlen) == 0;
    }

    inline const char* RepresentedData::get_path() const
    {
        return m_path;
    }

    inline const RepresentedData::data_storage* RepresentedData::get_in_time(const size_t p) const
    {
        if (p >= max_data_stored_array) return nullptr;
        auto* calc = m_current_data - p - 1;
        if (calc < m_data) calc += max_data_stored_array;
        return calc;
    }

    inline const RepresentedData::data_storage* RepresentedData::begin() const
    {
        return std::begin(m_data);
    }
    
    inline const RepresentedData::data_storage* RepresentedData::end() const
    {
        return std::end(m_data);
    }

    inline uint64_t RepresentedData::get_all_time_dist() const
    {
        const auto* newest = get_in_time(0);
        const auto* oldest = get_in_time(max_data_stored_array-1);

        return newest->get_modified_ms() - oldest->get_modified_ms();
    }



    inline std::shared_ptr<RepresentedData> StoredDataEachDevice::_find(const char* s) const
    {
        for(auto i = m_data.begin(); i != m_data.end(); ++i) {
            if ((*i)->is_path(s)) return (*i);
        }
        return {};
    }

    template<typename T>
    inline void StoredDataEachDevice::update_data(const char* s, const T& v)
    {
        m_has_new_data_for_display_update = true;
        std::shared_ptr<RepresentedData> it = _find(s);
        if (!it) {
            m_data.push_back(std::move(std::unique_ptr<RepresentedData>(new RepresentedData(s, v))));
            return;
        }
        it->set_value(v);
    }

    inline const std::shared_ptr<RepresentedData> StoredDataEachDevice::get(const char* s) const
    {
        return _find(s);
    }

    inline void StoredDataEachDevice::set_has_issues(const bool v)
    {
        m_has_new_data_for_display_update |= m_has_issues != v;
        m_has_issues = v;
    }

    inline void StoredDataEachDevice::set_is_online(const bool v)
    {
        m_has_new_data_for_display_update |= m_online != v;
        m_online = v;
    }

    inline bool StoredDataEachDevice::get_has_issues() const
    {
        return m_has_issues;
    }

    inline bool StoredDataEachDevice::get_is_online() const
    {
        return m_online;
    }


    inline size_t StoredDataEachDevice::size() const
    {
        return m_data.size();
    }

    inline std::shared_ptr<RepresentedData> StoredDataEachDevice::operator()(const size_t idx)
    {
        if (idx >= m_data.size()) return {};
        m_has_new_data_for_display_update = true;
        return *(std::next(m_data.begin(), idx));
    }

    inline const std::shared_ptr<RepresentedData> StoredDataEachDevice::operator[](const size_t idx) const
    {
        if (idx >= m_data.size()) return {};
        return *(std::next(m_data.begin(), idx));
    }
    
    inline StoredDataEachDevice::data_cptr StoredDataEachDevice::begin() const
    {
        return m_data.cbegin();
    }

    inline StoredDataEachDevice::data_cptr StoredDataEachDevice::end() const
    {
        return m_data.cend();
    }

    inline void StoredDataEachDevice::set_store_on_sd_card(uint32_t t)
    {
        m_store_sd_card = t;
    }

    inline void StoredDataEachDevice::set_send_to_influx_db(uint32_t t)
    {
        m_store_influx_db = t;
    }

    inline uint32_t StoredDataEachDevice::get_store_sd_card() const
    {
        return m_store_sd_card;
    }

    inline uint32_t StoredDataEachDevice::get_send_influx_db() const
    {
        return m_store_influx_db;
    }

    inline bool StoredDataEachDevice::has_new_data_for_display()
    {
        return EXC_RETURN(m_has_new_data_for_display_update, false);
    }



    inline void SIMData::set_time(const struct tm& ref)
    {
        m_copy_loc = ref;
        //m_copy_loc_as_time = mktime(&m_copy_loc);
        m_copy_loc_off = CPU::get_time_ms();
        m_has_new_data_for_display_update_of[static_cast<size_t>(test_has_new_data_of::TIME)] = true;
    }

    inline void SIMData::set_rssi(const int rssi)
    {
        m_rssi = rssi;
        m_has_new_data_for_display_update_of[static_cast<size_t>(test_has_new_data_of::RSSI)] = true;
    }

    inline struct tm SIMData::get_time() const
    {
        struct tm cpy = m_copy_loc;
        if (m_copy_loc_off != 0) {
            cpy.tm_sec += ((CPU::get_time_ms() - m_copy_loc_off) / 1000);
            mktime(&cpy);
        }
        return cpy;
    }

    /*inline time_t SIMData::get_time_t() const
    {
        return m_copy_loc_as_time + (m_copy_loc_off != 0 ? static_cast<time_t>((CPU::get_time_ms() - m_copy_loc_off) / 1000) : 0);
    }*/

    inline const char* SIMData::get_time(char* buf, size_t len, const SIMData::time_format format, const SIMData::time_type type)
    {
        const auto m_loc = get_time();

        switch(format) {
        case SIMData::time_format::CLOCK_FULL:
            if (type == SIMData::time_type::HOUR_12) {
                snprintf(buf, len, "%02i:%02i:%02i %s",
                    (m_loc.tm_hour % 12 == 0 ? 12 : m_loc.tm_hour % 12), m_loc.tm_min, m_loc.tm_sec, m_loc.tm_hour >= 12 ? "PM" : "AM");
            }
            else {
                snprintf(buf, len, "%02i:%02i:%02i",
                    m_loc.tm_hour, m_loc.tm_min, m_loc.tm_sec);
            }
            break;
        case SIMData::time_format::CLOCK_RESUMED:
            if (type == SIMData::time_type::HOUR_12) {
                snprintf(buf, len, "%02i:%02i %s",
                    (m_loc.tm_hour % 12 == 0 ? 12 : m_loc.tm_hour % 12), m_loc.tm_min, m_loc.tm_hour >= 12 ? "PM" : "AM");
            }
            else {
                snprintf(buf, len, "%02i:%02i",
                    m_loc.tm_hour, m_loc.tm_min);
            }
            break;
        case SIMData::time_format::DATE:
            snprintf(buf, len, "%04i/%02i/%02i", m_loc.tm_year + 1900, m_loc.tm_mon + 1, m_loc.tm_mday);
            break;
        case SIMData::time_format::BOTH_FULL:
            if (type == SIMData::time_type::HOUR_12) {
                snprintf(buf, len, "%04i/%02i/%02i %02i:%02i:%02i %s",
                    m_loc.tm_year + 1900, m_loc.tm_mon + 1, m_loc.tm_mday,
                    (m_loc.tm_hour % 12 == 0 ? 12 : m_loc.tm_hour % 12), m_loc.tm_min, m_loc.tm_sec, m_loc.tm_hour >= 12 ? "PM" : "AM");
            }
            else {
                snprintf(buf, len, "%04i/%02i/%02i %02i:%02i:%02i",
                    m_loc.tm_year + 1900, m_loc.tm_mon + 1, m_loc.tm_mday,
                    m_loc.tm_hour, m_loc.tm_min, m_loc.tm_sec);
            }
            break;
        }
        return buf;
    }

    inline int SIMData::get_rssi() const
    {
        return m_rssi;
    }

    inline bool SIMData::has_new_data_for_display(SIMData::test_has_new_data_of t)
    {
        return t == SIMData::test_has_new_data_of::_MAX ? false : EXC_RETURN(m_has_new_data_for_display_update_of[static_cast<size_t>(t)], false);
    }



    inline void InfluxDBData::set_is_online(const bool v)
    {
        m_online = v;
    }

    inline bool InfluxDBData::get_is_online() const
    {
        return m_online;
    }


/// FUTURE FUTURE FUTURE MAYBE NO TIME OH NO
///    inline SDStorageAuto::data_stored::data_stored(const char* s)
///    {
///        const size_t len = strlen(s);
///        if (len > 0) strncpy(m_data_path, s, sizeof(m_data_path));
///    }
///
///    inline bool SDStorageAuto::has_config(const std::string& key) const
///    {
///        const auto it = m_data_map.find(key);
///        return (it != m_data_map.end());
///    }
///
///    inline void SDStorageAuto::set_config(const std::string& key, const std::string& val)
///    {
///        std::lock_guard<std::mutex> l(m_mtx);
///        m_data_map[key] = val;
///        m_has_external_updated_it = true;
///    }
///
///    inline std::string SDStorageAuto::get_config(const std::string& key) const
///    {
///        std::lock_guard<std::mutex> l(m_mtx);
///        const auto it = m_data_map.find(key);
///        if (it != m_data_map.end()) return it->second;
///        return {};
///    }
///
///    template<typename T>
///    inline void SDStorageAuto::set_config_hex_t(const std::string& key, const T& val)
///    {
///        std::lock_guard<std::mutex> l(m_mtx);
///        auto str_val = std::unique_ptr<char[]>(new char[_to_hex_format(nullptr, val)]);
///        _to_hex_format(str_val.get(), val);
///        m_has_external_updated_it = true;
///        set_config(key, str_val)
///    }
///
///    template<typename T>
///    inline bool SDStorageAuto::get_config_hex_t(const std::string& key, T& val)
///    {
///        std::lock_guard<std::mutex> l(m_mtx);
///        const auto it = get_config(key);
///        return _from_hex_format(it->second.c_str(), val);
///    }
///
///    inline const char* SDStorageAuto::get_path() const
///    {
///        return m_data_path;
///    }
///
///    inline std::unique_lock<std::mutex> SDStorageAuto::get_lock()
///    {
///        return {m_mtx};
///    }
///
///    inline SDStorageAuto::map_cptr SDStorageAuto::begin() const
///    {
///        return m_data_map.begin();
///    }
///
///    inline SDStorageAuto::map_cptr SDStorageAuto::end() const
///    {
///        return m_data_map.end();
///    }
///
///
///
///
/////    inline void SDStorageAuto::_load_internally()
/////    {
/////        if (CPU::get_core_id() != cpu_core_id_for_sd_card) {
/////            LOGE(TAG, "SDStoreAuto got _load_internally running on the wrong core! Abort");
/////            return;
/////        }
/////
/////        if (!m_has_external_updated_it && m_has_service_loaded_it) return;
/////        std::lock_guard<std::mutex> l(m_mtx);
/////        m_has_external_updated_it = false;
/////        
/////        _recursive_pathing(m_data_path, [](const char* path){
/////            SDcard::mkdir(path);
/////        });
/////
/////        File fp = SDcard::f_open
/////
/////
/////        m_has_service_loaded_it = true;
/////    }




    inline StoredDataEachDevice& SharedData::operator()(const CS::device_id& id)
    {
        return m_devices[CS::d2u(id)];
    }

    const StoredDataEachDevice& SharedData::operator[](const CS::device_id& id) const
    {
        return m_devices[CS::d2u(id)];
    }
    
    inline const StoredDataEachDevice* SharedData::begin() const
    {
        return std::begin(m_devices);
    }

    inline const StoredDataEachDevice* SharedData::end() const
    {
        return std::end(m_devices);
    }

    inline const StoredDataEachDevice* SharedData::get_ptr_to(const CS::device_id& id) const
    {
        return &m_devices[CS::d2u(id)];
    }

    inline size_t SharedData::get_total_devices_online() const
    {
        size_t c = 0;
        for(auto& i : m_devices) {
            if (i.get_is_online()) ++c;
        }
        return c;
    }

    inline const SIMData& SharedData::get_sim_data() const
    {
        return m_sim;
    }

    inline SIMData& SharedData::get_sim_data()
    {
        return m_sim;
    }

    inline const InfluxDBData& SharedData::get_idb_data() const
    {
        return m_idb;
    }

    inline InfluxDBData& SharedData::get_idb_data()
    {
        return m_idb;
    }

    /*inline const SDStorageAuto& SharedData::get_sd_data() const
    {
        return m_asd;
    }

    inline SDStorageAuto& SharedData::get_sd_data()
    {
        return m_asd;
    }*/

}