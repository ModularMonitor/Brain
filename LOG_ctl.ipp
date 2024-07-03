#pragma once

#include "LOG_ctl.h"
#include "SD_card.h"


inline int MyLOG::printf(const e_LOG_TAG& tag, const e_LOG_TYPE& typ, const char* format, ...)
{
    std::lock_guard<std::recursive_mutex> l(m_mtx);
    if (!Serial) Serial.begin(logger_serial_speed);
    MySDcard& sd = GET(MySDcard);

    char buf_write[logger_buffer_len]{};

    try {
        const char* tag_s = "UNK";
        const char* typ_s = "U";

        switch(tag) {
        case e_LOG_TAG::TAG_MAIN:   tag_s = "MAN";      break;
        case e_LOG_TAG::TAG_SD:     tag_s = "SDC";      break;
        case e_LOG_TAG::TAG_CPU:    tag_s = "CPU";      break;
        case e_LOG_TAG::TAG_I2C:    tag_s = "I2C";      break;
        case e_LOG_TAG::TAG_CORE:   tag_s = "COR";      break;
        case e_LOG_TAG::TAG_CFG:    tag_s = "CFG";      break;
        default: break; // UNK
        }

        switch (typ) {
        case e_LOG_TYPE::INFO:      typ_s = "I";        break;
        case e_LOG_TYPE::WARN:      typ_s = "W";        break;
        case e_LOG_TYPE::ERROR:     typ_s = "E";        break;
        default: break; // U
        }

        int written = snprintf(buf_write, sizeof(buf_write) - 1, "@%i$%016llu %s %s: ", (int)xPortGetCoreID(), get_time_ms(), tag_s, typ_s);
        if (written < 0) written = 0;

        va_list args;
        va_start (args, format);
        const int res = vsnprintf(buf_write + written, sizeof(buf_write) - written - 1, format, args);
        va_end(args);

        const int siz = strnlen(buf_write, sizeof(buf_write) - 1);
        if (siz < 0) throw std::runtime_error("Log generated was too short somehow?");

        buf_write[siz] = '\n';

        Serial.printf("%.*s", siz + 1, buf_write);
        sd.async_append_on(logger_log_path, buf_write, siz + 1);

        return siz + 1;
    }
    catch(const std::exception& e) {
        const int siz = snprintf(buf_write, sizeof(buf_write), "EXC @%i$%llu: %s\n", (int)xPortGetCoreID(), get_time_ms(), e.what());

        Serial.printf("%.*s", siz, buf_write);
        sd.async_append_on(logger_exception_path, buf_write, siz);
    }
    catch(...) {
        const int siz = snprintf(buf_write, sizeof(buf_write), "EXC @%i$%llu: UNCAUGHT\n", (int)xPortGetCoreID(), get_time_ms());

        Serial.printf("%.*s", siz, buf_write);
        sd.async_append_on(logger_exception_path, buf_write, siz);
    }
    return 0;
}

inline int MyLOG::printf_nosd(const e_LOG_TAG& tag, const e_LOG_TYPE& typ, const char* format, ...)
{
    std::lock_guard<std::recursive_mutex> l(m_mtx);
    if (!Serial) Serial.begin(logger_serial_speed);

    char buf_write[logger_buffer_len]{};

    try {
        const char* tag_s = "UNK";
        const char* typ_s = "U";

        switch(tag) {
        case e_LOG_TAG::TAG_MAIN:   tag_s = "MAN";      break;
        case e_LOG_TAG::TAG_SD:     tag_s = "SDC";      break;
        case e_LOG_TAG::TAG_CPU:    tag_s = "CPU";      break;
        case e_LOG_TAG::TAG_I2C:    tag_s = "I2C";      break;
        case e_LOG_TAG::TAG_CORE:   tag_s = "COR";      break;
        case e_LOG_TAG::TAG_CFG:    tag_s = "CFG";      break;
        default: break; // UNK
        }

        switch (typ) {
        case e_LOG_TYPE::INFO:      typ_s = "I";        break;
        case e_LOG_TYPE::WARN:      typ_s = "W";        break;
        case e_LOG_TYPE::ERROR:     typ_s = "E";        break;
        default: break; // U
        }

        int written = snprintf(buf_write, sizeof(buf_write) - 1, "!%i$%016llu %s %s: ", (int)xPortGetCoreID(), get_time_ms(), tag_s, typ_s);
        if (written < 0) written = 0;

        va_list args;
        va_start (args, format);
        const int res = vsnprintf(buf_write + written, sizeof(buf_write) - written - 1, format, args);
        va_end(args);

        const int siz = strnlen(buf_write, sizeof(buf_write) - 1);
        if (siz < 0) throw std::runtime_error("Log generated was too short somehow?");

        buf_write[siz] = '\n';

        Serial.printf("%.*s", siz + 1, buf_write);

        return siz + 1;
    }
    catch(const std::exception& e) {
        const int siz = snprintf(buf_write, sizeof(buf_write), "!EXC @%i$%llu: %s\n", (int)xPortGetCoreID(), get_time_ms(), e.what());

        Serial.printf("%.*s", siz, buf_write);
    }
    catch(...) {
        const int siz = snprintf(buf_write, sizeof(buf_write), "!EXC @%i$%llu: UNCAUGHT\n", (int)xPortGetCoreID(), get_time_ms());

        Serial.printf("%.*s", siz, buf_write);
    }
    return 0;
}