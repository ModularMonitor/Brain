#pragma once

#include <Arduino.h>

#include "defaults.h"
#include "SD_card.h"

//constexpr char webserver_home[] = R"===(
//<!DOCTYPE html><html><head><title>ESP32 CaptivePortal</title></head><body>
//<h1>Hello World!</h1><p>This is a captive portal example page.</p></body></html>
//)===";


inline String get_webserver_home()
{
    auto& sd = GET(MySDcard);
    String buf;
    char tmp_buf[web_slice_read];
    uint32_t off = 0, current_read = 0;
    while(1) {
        current_read = sd.read_from(web_file_html, tmp_buf, web_slice_read, off);
        off += current_read;

        buf += String(tmp_buf, current_read);

        if (current_read < web_slice_read) break;
    }

    return buf;
}

inline String get_webserver_js()
{
    auto& sd = GET(MySDcard);
    String buf;
    char tmp_buf[web_slice_read];
    uint32_t off = 0, current_read = 0;
    while(1) {
        current_read = sd.read_from(web_file_js, tmp_buf, web_slice_read, off);
        off += current_read;

        buf += String(tmp_buf, current_read);

        if (current_read < web_slice_read) break;
    }

    return buf;
}

inline String get_webserver_css()
{
    auto& sd = GET(MySDcard);
    String buf;
    char tmp_buf[web_slice_read];
    uint32_t off = 0, current_read = 0;
    while(1) {
        current_read = sd.read_from(web_file_css, tmp_buf, web_slice_read, off);
        off += current_read;

        buf += String(tmp_buf, current_read);

        if (current_read < web_slice_read) break;
    }

    return buf;
}