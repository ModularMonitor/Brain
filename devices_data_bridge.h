#pragma once

// implement a map of devices and their data here + if they
// are online + interval to send data of each to influxdb

#include "Serial/packaging.h"
#include <string>
#include <unordered_map>

constexpr size_t max_data_saved = 60;

using namespace CS;

class ModuleMapping {
public:
    struct module_data_t {
        union {
            double d;
            float f;
            int64_t i;
            uint64_t u;
        } v{};
        uint8_t k = static_cast<uint8_t>(-1);
        module_data_t()             : k(static_cast<uint8_t>(-1)) { v.d = 0.0; }
        module_data_t(double d)     : k(0) { v.d = d; }
        module_data_t(float f)      : k(1) { v.f = f; }
        module_data_t(int64_t i)    : k(2) { v.i = i; }
        module_data_t(uint64_t u)   : k(3) { v.u = u; }
        module_data_t& operator=(double d)   { v.d = d; k = 0; return *this; }
        module_data_t& operator=(float f)    { v.f = f; k = 1; return *this; }
        module_data_t& operator=(int64_t i)  { v.i = i; k = 2; return *this; }
        module_data_t& operator=(uint64_t u) { v.u = u; k = 3; return *this; }
        bool is_d() const { return k == 0; }
        bool is_f() const { return k == 1; }
        bool is_i() const { return k == 2; }
        bool is_u() const { return k == 3; }
    };

    struct module_data_circular {
        module_data_t m_data[max_data_saved];
        size_t m_offset = 0;

        void push(module_data_t val) {
            m_data[m_offset] = val;
            // not done directly so multithread won't break
            if (m_offset + 1 >= max_data_saved) m_offset = 0;
            else ++m_offset;
        }
        // get from newest to oldest. 0 == now, 1 == last, ..., max_data_saved-1 == oldest
        module_data_t get_newest(size_t off = 0) const {
            if (off >= max_data_saved) return {}; // please respect limits

            size_t exp = m_offset - 1 - off; // may be negative, or, in other words, a huge number
            if (exp >= max_data_saved) exp += max_data_saved; // if 0 and want 1, number will explode. then, .... + max_data_saved = max_data_saved - 1.

            return m_data[exp];
        }
        
        // get from oldest to newest. 0 == oldest, 1 == less old, ..., max_data_saved-1 == newest
        module_data_t get_oldest(size_t off = 0) const {
            if (off >= max_data_saved) return {}; // please respect limits

            size_t exp = m_offset + off;
            if (exp >= max_data_saved) exp -= max_data_saved; // may be 60, so 0 etc
            
            return m_data[exp];
        }
    };

    using module_each_map_t = std::unordered_map<std::string, module_data_circular>; // a path and a value array
    struct module_info {
        module_each_map_t m_data_map;
        bool is_online = false;
        bool has_issues = false;
    };
    using module_map_t = std::unordered_map<device_id, module_info>; // a module may have 0..n module_info (contains info about itself + mapping of info). this lists all modules
private:
    module_map_t m_map;
public:
    // simple way to tell if module is on
    void set_online(device_id who, bool on) {
        m_map[who].is_online = on;
    }
    // get if module is up and running (connected)
    bool get_online(device_id who) {
        return m_map[who].is_online;
    }
    // simple way to tell if module is on
    void set_has_issues(device_id who, bool on) {
        m_map[who].has_issues = on;
    }
    // get if module is up and running (connected)
    bool get_has_issues(device_id who) {
        return m_map[who].has_issues;
    }
    // add new data on path to module
    void push_data_for(device_id who, const char* path, const module_data_t val) {
        m_map[who].m_data_map[std::string(path)].push(val);
    }
    // get all paths and values of module
    const module_each_map_t& get_data_of(device_id who) {
        return m_map[who].m_data_map;
    }
};

ModuleMapping& get_global_map() {
    static ModuleMapping map;
    return map;
}