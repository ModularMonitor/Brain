#pragma once

#include "data_displayed.h"

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

namespace STR {

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



    inline RepresentedData* StoredDataEachDevice::_find(const char* s) const
    {
        for(auto i = m_data.begin(); i != m_data.end(); ++i) {
            if ((*i)->is_path(s)) return i->get();
        }
        return nullptr;
    }

    template<typename T>
    inline void StoredDataEachDevice::update_data(const char* s, const T& v)
    {
        RepresentedData* it = _find(s);
        if (!it) {
            m_data.push_back(std::move(std::unique_ptr<RepresentedData>(new RepresentedData(s, v))));
            return;
        }
        it->set_value(v);
    }

    inline const RepresentedData* StoredDataEachDevice::get(const char* s) const
    {
        return _find(s);
    }

    inline void StoredDataEachDevice::set_has_issues(const bool v)
    {
        m_has_issues = v;
    }

    inline void StoredDataEachDevice::set_is_online(const bool v)
    {
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

    inline RepresentedData* StoredDataEachDevice::operator()(const size_t idx)
    {
        if (idx >= m_data.size()) return nullptr;
        return (m_data.begin() + idx)->get();
    }

    inline const RepresentedData* StoredDataEachDevice::operator[](const size_t idx) const
    {
        if (idx >= m_data.size()) return nullptr;
        return (m_data.begin() + idx)->get();
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



    inline void SIMData::set_time(const struct tm& ref)
    {
        m_copy_loc = ref;
        //m_copy_loc_as_time = mktime(&m_copy_loc);
        m_copy_loc_off = CPU::get_time_ms();
        m_has_new_data_of[static_cast<size_t>(test_has_new_data_of::TIME)] = true;
    }

    inline void SIMData::set_rssi(const int rssi)
    {
        m_rssi = rssi;
        m_has_new_data_of[static_cast<size_t>(test_has_new_data_of::RSSI)] = true;
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

    inline bool SIMData::has_new_data_of(SIMData::test_has_new_data_of t)
    {
        return t == SIMData::test_has_new_data_of::_MAX ? false : EXC_RETURN(m_has_new_data_of[static_cast<size_t>(t)], false);
    }




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

    inline const SIMData& SharedData::get_sim_data() const
    {
        return m_sim;
    }

    inline SIMData& SharedData::get_sim_data()
    {
        return m_sim;
    }


}