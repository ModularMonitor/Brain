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
    TAG_CPU
};

enum class e_LOG_TYPE{
    INFO,
    WARN,
    ERROR
};

// use GET(MyLOG) to get its singleton ref!
MAKE_SINGLETON_CLASS(MyLOG, {
    std::mutex m_mtx;
public:
    int printf(const e_LOG_TAG&, const e_LOG_TYPE&, const char*, ...);
});

#define LOGI(TAG, ...) GET(MyLOG).printf(TAG, e_LOG_TYPE::INFO,  __VA_ARGS__)
#define LOGW(TAG, ...) GET(MyLOG).printf(TAG, e_LOG_TYPE::WARN,  __VA_ARGS__)
#define LOGE(TAG, ...) GET(MyLOG).printf(TAG, e_LOG_TYPE::ERROR, __VA_ARGS__)