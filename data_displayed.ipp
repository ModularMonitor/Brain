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

    void RepresentedData::data_storage::set_value(const double& v)
    {
        snprintf(m_value_str, sizeof(m_value_str), "%.8lf", v);
        m_update_time = CPU::get_time_ms();
    }

    void RepresentedData::data_storage::set_value(const float& v)
    {
        snprintf(m_value_str, sizeof(m_value_str), "%.6f", v);
        m_update_time = CPU::get_time_ms();
    }

    void RepresentedData::data_storage::set_value(const int64_t& v)
    {
        snprintf(m_value_str, sizeof(m_value_str), "%" PRId64, v);
        m_update_time = CPU::get_time_ms();
    }

    void RepresentedData::data_storage::set_value(const uint64_t& v)
    {
        snprintf(m_value_str, sizeof(m_value_str), "%" PRIu64, v);
        m_update_time = CPU::get_time_ms();
    }

    const char* RepresentedData::data_storage::get_value() const
    {
        return m_value_str;
    }

    uint64_t RepresentedData::data_storage::get_modified_ms() const
    {
        return m_update_time;
    }


    void RepresentedData::advance_ptr()
    {
        if (++m_current_data >= (m_data + max_data_stored_array)) m_current_data = m_data;
    }

    template<typename T>
    RepresentedData::RepresentedData(const char* s, const T& v)
    {
        size_t len = strlen(s);
        if (len <= 0) return;
        if (len > CS::max_path_len) len = CS::max_path_len;
        memcpy(m_path, s, len);

        set_value(v);
    }

    template<typename T>
    void RepresentedData::set_value(const T& v)
    {
        m_current_data->set_value(v);
        advance_ptr();
    }

    bool RepresentedData::is_path(const char* s) const
    {
        size_t len = strlen(s);
        size_t mlen = strlen(m_path);
        if (len <= 0 || len > CS::max_path_len || mlen != len) return false;
        return memcmp(m_path, s, mlen) == 0;
    }

    const char* RepresentedData::get_path() const
    {
        return m_path;
    }

    const RepresentedData::data_storage* RepresentedData::get_in_time(const size_t p) const
    {
        if (p >= max_data_stored_array) return nullptr;
        auto* calc = m_current_data - p - 1;
        if (calc < m_data) calc += max_data_stored_array;
        return calc;
    }

    const RepresentedData::data_storage* RepresentedData::begin() const
    {
        return std::begin(m_data);
    }
    
    const RepresentedData::data_storage* RepresentedData::end() const
    {
        return std::end(m_data);
    }

    uint64_t RepresentedData::get_all_time_dist() const
    {
        const auto* newest = get_in_time(0);
        const auto* oldest = get_in_time(max_data_stored_array-1);

        return newest->get_modified_ms() - oldest->get_modified_ms();
    }



    RepresentedData* StoredDataEachDevice::_find(const char* s) const
    {
        for(auto i = m_data.begin(); i != m_data.end(); ++i) {
            if ((*i)->is_path(s)) return i->get();
        }
        return nullptr;
    }

    template<typename T>
    void StoredDataEachDevice::update_data(const char* s, const T& v)
    {
        RepresentedData* it = _find(s);
        if (!it) {
            m_data.push_back(std::move(std::unique_ptr<RepresentedData>(new RepresentedData(s, v))));
            return;
        }
        it->set_value(v);
    }

    const RepresentedData* StoredDataEachDevice::get(const char* s) const
    {
        return _find(s);
    }

    void StoredDataEachDevice::set_has_issues(const bool v)
    {
        m_has_issues = v;
    }

    void StoredDataEachDevice::set_is_online(const bool v)
    {
        m_online = v;
    }

    bool StoredDataEachDevice::get_has_issues() const
    {
        return m_has_issues;
    }

    bool StoredDataEachDevice::get_is_online() const
    {
        return m_online;
    }


    size_t StoredDataEachDevice::size() const
    {
        return m_data.size();
    }

    RepresentedData* StoredDataEachDevice::operator()(const size_t idx)
    {
        if (idx >= m_data.size()) return nullptr;
        return (m_data.begin() + idx)->get();
    }

    const RepresentedData* StoredDataEachDevice::operator[](const size_t idx) const
    {
        if (idx >= m_data.size()) return nullptr;
        return (m_data.begin() + idx)->get();
    }
    
    StoredDataEachDevice::data_cptr StoredDataEachDevice::begin() const
    {
        return m_data.cbegin();
    }

    StoredDataEachDevice::data_cptr StoredDataEachDevice::end() const
    {
        return m_data.cend();
    }

    StoredDataEachDevice& SharedData::operator()(const CS::device_id& id)
    {
        return m_devices[CS::d2u(id)];
    }

    const StoredDataEachDevice& SharedData::operator[](const CS::device_id& id) const
    {
        return m_devices[CS::d2u(id)];
    }
    
    const StoredDataEachDevice* SharedData::begin() const
    {
        return std::begin(m_devices);
    }

    const StoredDataEachDevice* SharedData::end() const
    {
        return std::end(m_devices);
    }

    const StoredDataEachDevice* SharedData::get_ptr_to(const CS::device_id& id) const
    {
        return &m_devices[CS::d2u(id)];
    }

}