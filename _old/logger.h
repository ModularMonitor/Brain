#pragma once

#include "common.h"

#include <stdint.h>
#include <mutex>

namespace LG {

    constexpr size_t log_line_dist = 20;
    constexpr size_t log_amount = 320 / log_line_dist;
    constexpr size_t log_line_max_len = 96;


    MAKE_SINGLETON_CLASS(Logger, {
        char m_lines_buffering[log_amount][log_line_max_len]{};
        bool m_had_news = false;
        std::mutex m_mtx;
    public:
        Logger();
        
        int printf(const char*, const char*, ...);

        // 0 == most recent, log_amount-1 == oldest
        const char* get_line(const size_t) const;

        // just log_amount
        size_t size() const;

        bool for_display_had_news();
    });
}

#define LOGI(TAG, ...) ::LG::get_singleton_of_Logger().printf(TAG, __VA_ARGS__)
#define LOGW(TAG, ...) ::LG::get_singleton_of_Logger().printf(TAG, __VA_ARGS__)
#define LOGE(TAG, ...) ::LG::get_singleton_of_Logger().printf(TAG, __VA_ARGS__)

#include "logger.ipp"