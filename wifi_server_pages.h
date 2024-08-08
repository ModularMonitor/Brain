#pragma once

#include <Arduino.h>

#include "defaults.h"
#include "SD_card.h"

//constexpr char webserver_home[] = R"===(
//<!DOCTYPE html><html><head><title>ESP32 CaptivePortal</title></head><body>
//<h1>Hello World!</h1><p>This is a captive portal example page.</p></body></html>
//)===";

inline String& _get_webserver_idx(const int off) {
    static String pages[3];
    return pages[off];
}

inline void reload_webserver_items() {
    auto& sd = GET(MySDcard);

    for(int i = 0; i < 3; ++i) {
        String& buf = _get_webserver_idx(i);
        buf.clear();

        char tmp_buf[web_slice_read];
        size_t off = 0, current_read = 0;

        const char* page = [&i]() -> const char* { switch(i){
            case 0: return web_file_html;
            case 1: return web_file_js;
            default: return web_file_css;
        }}();

        const size_t file_expected_size = sd.get_file_size(page);

        LOGI(e_LOG_TAG::TAG_WIFI, "Reloading webpage index %i '%s' of size %zu...", i, page, file_expected_size);

        while(off < file_expected_size) {
            current_read = sd.read_from(page, tmp_buf, web_slice_read, off);
            off += current_read;
            if (current_read) {
                buf += String(tmp_buf, current_read);
            }
            else {
                LOGW_NOSD(e_LOG_TAG::TAG_WIFI, "Got zero bytes read. Current: %zu, progress: %.2f%%. Waiting a second before retry.", off, (100.0f * off / file_expected_size));
                SLEEP(1000);
            }
        }

        LOGI(e_LOG_TAG::TAG_WIFI, "Reloaded webpage index %i '%s' now with size %u.", i, page, buf.length());
    }
}

inline const String& get_webserver_home()
{
    String& ref = _get_webserver_idx(0);
    if (ref.isEmpty()) reload_webserver_items();
    return ref;
}

inline const String& get_webserver_js()
{
    String& ref = _get_webserver_idx(1);
    if (ref.isEmpty()) reload_webserver_items();
    return ref;
}

inline const String& get_webserver_css()
{
    String& ref = _get_webserver_idx(2);
    if (ref.isEmpty()) reload_webserver_items();
    return ref;
}