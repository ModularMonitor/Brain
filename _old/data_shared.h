#pragma once

#include "Serial/packaging.h"

#include "cpu_ctl.h"

#include <deque>
#include <memory>
#include <unordered_map>

namespace STR {

    static const char TAG[] = "DataShared";

    constexpr size_t max_data_stored_array = 60;

    // Each path value history
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

    // The information about the sensors, each of them
    class StoredDataEachDevice {
        std::deque<std::shared_ptr<RepresentedData>> m_data;

        // user settings
        uint32_t m_store_sd_card = 0; // 0 means no, else [seconds]
        uint32_t m_store_influx_db = 0; // 0 means no, else [seconds]

        // remaining props
        bool m_online = false;
        bool m_has_issues = false;

        // FOR DISPLAY!
        bool m_has_new_data_for_display_update = false;

        std::shared_ptr<RepresentedData> _find(const char*) const;
    public:
        using data_cptr = std::deque<std::shared_ptr<RepresentedData>>::const_iterator;

        template<typename T> 
        void update_data(const char*, const T&);
        const std::shared_ptr<RepresentedData> get(const char*) const;

        void set_has_issues(const bool);
        void set_is_online(const bool);

        bool get_has_issues() const;
        bool get_is_online() const;

        size_t size() const;

        std::shared_ptr<RepresentedData> operator()(const size_t);
        const std::shared_ptr<RepresentedData> operator[](const size_t) const;
        
        data_cptr begin() const;
        data_cptr end() const;

        // zero means no
        void set_store_on_sd_card(uint32_t);
        // zero means no
        void set_send_to_influx_db(uint32_t);

        uint32_t get_store_sd_card() const;
        uint32_t get_send_influx_db() const;

        bool has_new_data_for_display();
    };

    // All data from 4G
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

        // for DISPLAY!
        bool m_has_new_data_for_display_update_of[static_cast<size_t>(test_has_new_data_of::_MAX)];
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
        bool has_new_data_for_display(test_has_new_data_of);
    };

    // Related to InfluxDB information
    class InfluxDBData {
        bool m_online = false;
    public:
        void set_is_online(const bool);
        bool get_is_online() const;
    };

/// FUTURE FUTURE FUTURE MAYBE NO TIME OH NO
///    // SD card saved stuff, asynchronously. This way, classes like Display can just set things here and expect them to be saved
///    // All settings here WILL BE loaded and saved asynchronously by SDcard task.
///    class SDStorageAuto {
///    public:
///        class data_stored {
///            char m_data_path[128]{};
///            std::unordered_map<std::string, std::string> m_data_map; // key=value, where key must be alphanumeric.
///            std::mutex m_mtx;
///            bool m_has_service_loaded_it = false; // if service loaded it, who wants it may read them.
///            volatile bool m_has_external_updated_it = false; // if external set it, service should save it
///        public:
///            using map_cptr = std::unordered_map<std::string, std::string>::const_iterator;
///
///            data_stored(const char*); // must have path determined
///
///            bool has_config(const std::string&) const;
///            void set_config(const std::string&, const std::string&);
///            std::string get_config(const std::string&) const;
///            
///            template<typename T>
///            void set_config_hex_t(const std::string&, const T&);
///            template<typename T>
///            bool get_config_hex_t(const std::string&, T&);
///
///            const char* get_path() const;
///
///            bool _has_to_read_file();
///            bool _has_to_write_from_file();
///
///            std::unique_lock<std::mutex> get_lock();
///            map_cptr begin() const;
///            map_cptr end() const;
///
///
///            // used by sdcard service.
///            //void _load_internally();
///        };
///
///        enum class configs {
///            DISPLAY_CONFIG
///        };
///    private:
///        data_stored displayConfig[] = {
///            {"/configs/display.cfg"} /* DISPLAY_CONFIG: {} */
///        };
///    public:
///        const data_stored& operator[](const size_t) const;
///        data_stored& operator[](const size_t);
///    };



    MAKE_SINGLETON_CLASS(SharedData, {
        StoredDataEachDevice m_devices[CS::d2u(CS::device_id::_MAX)]{};
        SIMData m_sim;
        InfluxDBData m_idb;
        /*SDStorageAuto m_asd;*/
    public:
        StoredDataEachDevice& operator()(const CS::device_id&);
        const StoredDataEachDevice& operator[](const CS::device_id&) const;

        const StoredDataEachDevice* begin() const;
        const StoredDataEachDevice* end() const;

        const StoredDataEachDevice* get_ptr_to(const CS::device_id&) const;

        size_t get_total_devices_online() const;

        const SIMData& get_sim_data() const;
        SIMData& get_sim_data();

        const InfluxDBData& get_idb_data() const;
        InfluxDBData& get_idb_data();

        /*const SDStorageAuto& get_sd_data() const;
        SDStorageAuto& get_sd_data();*/
    });
    

}

#include "data_shared.ipp"