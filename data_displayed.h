#pragma once

#include "Serial/packaging.h"

#include "cpu_ctl.h"

#include <deque>
#include <memory>

namespace STR {


    constexpr size_t max_data_stored_array = 60;

    class RepresentedData {
    public:
        class data_storage {
            char m_value_str[32]{};
            uint64_t m_update_time = 0;
        public:
            data_storage() = default;
            
            template<typename T>
            data_storage(const T&);

            void set_value(const double&);
            void set_value(const float&);
            void set_value(const int64_t&);
            void set_value(const uint64_t&);

            const char* get_value() const;
            uint64_t get_modified_ms() const;
        };
    private:
        char m_path[CS::max_path_len]{};
        data_storage m_data[max_data_stored_array];
        data_storage* m_current_data = m_data;

        void advance_ptr();
    public:
        template<typename T>
        RepresentedData(const char*, const T&);

        template<typename T>
        void set_value(const T&);

        bool is_path(const char*) const;
        const char* get_path() const;

        // 0 == now, 1 == last, ...
        const data_storage* get_in_time(const size_t) const;

        const data_storage* begin() const;
        const data_storage* end() const;

        uint64_t get_all_time_dist() const;
    };


    class StoredDataEachDevice {
        std::deque<std::unique_ptr<RepresentedData>> m_data;
        bool m_online = false;
        bool m_has_issues = false;

        // user settings
        uint32_t m_store_sd_card = 0; // 0 means no, else [seconds]
        uint32_t m_store_influx_db = 0; // 0 means no, else [seconds]

        RepresentedData* _find(const char*) const;
    public:
        using data_cptr = std::deque<std::unique_ptr<RepresentedData>>::const_iterator;

        template<typename T> 
        void update_data(const char*, const T&);
        const RepresentedData* get(const char*) const;

        void set_has_issues(const bool);
        void set_is_online(const bool);

        bool get_has_issues() const;
        bool get_is_online() const;

        size_t size() const;

        RepresentedData* operator()(const size_t);
        const RepresentedData* operator[](const size_t) const;
        
        data_cptr begin() const;
        data_cptr end() const;

        // zero means no
        void set_store_on_sd_card(uint32_t);
        // zero means no
        void set_send_to_influx_db(uint32_t);

        uint32_t get_store_sd_card() const;
        uint32_t get_send_influx_db() const;
    };

    class SIMData {
    public:
        enum class test_has_new_data_of {
            TIME,
            RSSI,
            _MAX
        };
        enum class time_format{
            CLOCK_FULL,     // xx:xx:xx
            CLOCK_RESUMED,  // xx:xx
            DATE,           // xxxx/xx/xx
            BOTH_FULL       // xxxx/xx/xx xx:xx:xx
        };
        enum class time_type {
            HOUR_24, // 00-23 clock
            HOUR_12  // Appends AM/PM in the end
        };
    private:
        struct tm m_copy_loc{};
        uint64_t m_copy_loc_off = 0;
        //time_t m_copy_loc_as_time = 0;
        int m_rssi = -1; // -1 == unknown; ranges (good to bad bars): [ 0..64, 65..74, 75..84, 85..94, 95..inf] (lib shows positive, but it is negative irl)

        bool m_has_new_data_of[static_cast<size_t>(test_has_new_data_of::_MAX)];
    public:
        // used by SIM to set current time
        void set_time(const struct tm&);
        // used by SIM to set rssi
        void set_rssi(const int);

        // get time set by SIM
        struct tm get_time() const;
        //time_t get_time_t() const;
        const char* get_time(char*, size_t, const time_format = time_format::BOTH_FULL, const time_type = time_type::HOUR_12);
        // get RSSI got by SIM
        int get_rssi() const;

        // automatic -> false test to check if has new data of type.
        bool has_new_data_of(test_has_new_data_of);
    };

    MAKE_SINGLETON_CLASS(SharedData, {
        StoredDataEachDevice m_devices[CS::d2u(CS::device_id::_MAX)]{};
        SIMData m_sim;
    public:
        StoredDataEachDevice& operator()(const CS::device_id&);
        const StoredDataEachDevice& operator[](const CS::device_id&) const;

        const StoredDataEachDevice* begin() const;
        const StoredDataEachDevice* end() const;

        const StoredDataEachDevice* get_ptr_to(const CS::device_id&) const;

        const SIMData& get_sim_data() const;
        SIMData& get_sim_data();
    });
    

}

#include "data_displayed.ipp"