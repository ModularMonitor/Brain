/*
LOG_ctl.h

# Depends on:
- defaults.h
- SD_card.h

# Description:
- This header contains the implementation of a logger, which is a singleton with mutex to make sure Serial and other stuff
don't overlap. It'll create a log.txt on the SD card, if any is found. It won't check for SD card write failures.

*/

#pragma once

#include "defaults.h"

#include <string>
#include <mutex>

enum class e_LOG_TAG{
    TAG_MAIN,
    TAG_SD,
    TAG_CPU,
    TAG_I2C,
    TAG_CORE,
    TAG_CFG,
    TAG_WIFI,
    TAG_STDIN
};

enum class e_LOG_TYPE{
    INFO,
    WARN,
    ERROR
};

// use GET(MyLOG) to get its singleton ref!
MAKE_SINGLETON_CLASS(MyLOG, {
    std::recursive_mutex m_mtx;
public:
    int printf(const e_LOG_TAG&, const e_LOG_TYPE&, const char*, ...);
    int printf_nosd(const e_LOG_TAG&, const e_LOG_TYPE&, const char*, ...);
    int printf_fordatalog(const e_LOG_TAG&, const char*, ...);
    int printf_nosd_raw(const char*, ...);
});

#define LOGI(TAG, ...) GET(MyLOG).printf(TAG, e_LOG_TYPE::INFO,  __VA_ARGS__)
#define LOGW(TAG, ...) GET(MyLOG).printf(TAG, e_LOG_TYPE::WARN,  __VA_ARGS__)
#define LOGE(TAG, ...) GET(MyLOG).printf(TAG, e_LOG_TYPE::ERROR, __VA_ARGS__)
#define LOGI_NOSD(TAG, ...) GET(MyLOG).printf_nosd(TAG, e_LOG_TYPE::INFO,  __VA_ARGS__)
#define LOGW_NOSD(TAG, ...) GET(MyLOG).printf_nosd(TAG, e_LOG_TYPE::WARN,  __VA_ARGS__)
#define LOGE_NOSD(TAG, ...) GET(MyLOG).printf_nosd(TAG, e_LOG_TYPE::ERROR, __VA_ARGS__)
#define LOG_DATA(TAG, ...) GET(MyLOG).printf_fordatalog(TAG, __VA_ARGS__)
#define LOGRAW(...) GET(MyLOG).printf_nosd_raw(__VA_ARGS__)