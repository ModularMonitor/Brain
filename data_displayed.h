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
    };

    MAKE_SINGLETON_CLASS(SharedData, {
        StoredDataEachDevice m_devices[CS::d2u(CS::device_id::_MAX)]{};
    public:
        StoredDataEachDevice& operator()(const CS::device_id&);
        const StoredDataEachDevice& operator[](const CS::device_id&) const;

        const StoredDataEachDevice* begin() const;
        const StoredDataEachDevice* end() const;

        const StoredDataEachDevice* get_ptr_to(const CS::device_id&) const;
    });
    

}

#include "data_displayed.ipp"