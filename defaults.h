#pragma once

#include "Serial/packaging.h"
#include "Serial/flags.h"

#include <stdint.h>
#include "esp32-hal.h"
#include "esp32-hal-cpu.h"
#include "soc/rtc.h"
#include "esp_freertos_hooks.h"

#include <chrono>
#include <vector>
#include <functional>

constexpr char app_version[] = "V0.0.1-BETA";

constexpr const uint8_t def_spi_core_id = 0; // spi (tft screen, touch)
constexpr const uint8_t def_alt_core_id = 1; // other tasks (+ sd card)

constexpr const auto& cpu_core_id_for_i2c           = def_alt_core_id;
constexpr const auto& cpu_core_id_for_core          = def_spi_core_id;
constexpr const auto& cpu_core_id_for_display_pwm   = def_alt_core_id;
constexpr const auto& cpu_core_id_for_sd_card       = def_alt_core_id;
constexpr const auto& cpu_core_id_for_serialstdin   = def_alt_core_id;
constexpr const auto& cpu_core_id_for_wifi_setup    = def_alt_core_id;
//constexpr const auto& cpu_core_id_for_4g_lte  = def_alt_core_id;

// ---- ---- WEBPAGE DEFAULTS BLOCK ---- ---- //

constexpr char web_file_html[] = "/webpage/index.html";
constexpr char web_file_js[]   = "/webpage/js.js";
constexpr char web_file_css[]  = "/webpage/css.css";
constexpr size_t web_slice_read = 32;
constexpr size_t web_timeout_write = 5000; // msec

// ---- ---- END OF WEBPAGE DEFAULTS BLOCK ---- ---- //

// ---- ---- SERIAL STDIN DEFAULTS BLOCK ---- ---- //

constexpr size_t serialstdin_buffer_size = 96;
constexpr size_t serialstdin_readblock_buffer_size = 256;
constexpr int serialstdin_thread_priority = 1; // higher is more important.

// ---- ---- END OF SERIAL STDIN DEFAULTS BLOCK ---- ---- //

// ---- ---- CONFIGURATION DEFAULTS BLOCK ---- ---- //

constexpr char config_file_path[] = "/config.ini";

// ---- ---- END OF CONFIGURATION DEFAULTS BLOCK ---- ---- //

// ---- ---- COREDISPLAY DEFAULTS BLOCK ---- ---- //

#define DISPLAY_CS         23
#define DISPLAY_RST        22
#define DISPLAY_DC         21
#define DISPLAY_MOSI       19
#define DISPLAY_SCLK       5
// DISPLAY_MISO disconnected (SDO)
// TOUCH_CLK = DISPLAY_SCLK
#define DISPLAY_TOUCH_CS   16
// TOUCH_DIN = DISPLAY_MOSI
#define DISPLAY_MISO       18

constexpr int core_display_led_pwm_pin = 17;
constexpr uint64_t core_display_led_pwn_delay = 35; // ms
constexpr uint64_t core_display_main_delay = 50; // ms
constexpr char core_display_config_ini[] = "/display_calibration.ini";
constexpr uint64_t core_display_screen_saver_steps_time = 10000; // ms
constexpr int core_thread_priority = tskIDLE_PRIORITY;
constexpr int core_led_pwm_thread_priority = tskIDLE_PRIORITY;
constexpr uint64_t core_display_animations_check_time = 1000; // ms, updates screen stuff.

namespace DisplayColors {
    //constexpr size_t log_line_dist = 20;
    //constexpr size_t log_amount = 320 / log_line_dist;
    //constexpr size_t log_line_max_len = 96;

    constexpr int32_t bar_top_height = 20;
    constexpr int32_t bar_right_width = 40;

    constexpr int32_t display_real_work_area[2] = {(480 - bar_right_width), (320 - bar_top_height) };

    constexpr int32_t bar_right_buttons_amount = 5;
    constexpr int32_t bar_right_each_height = display_real_work_area[1] / bar_right_buttons_amount;

    constexpr uint16_t bar_top_color = 0x34da;
    constexpr uint16_t bar_right_color = 0xcd49;
    constexpr uint16_t body_color = 0xFFFF;


    constexpr int32_t item_resumed_amount_on_screen = 5;
    constexpr int32_t item_resumed_height_max = display_real_work_area[1] / item_resumed_amount_on_screen;
    constexpr int32_t item_resumed_total_amount = display_real_work_area[1] / item_resumed_height_max;
    constexpr int32_t item_resumed_max_offset = (static_cast<int32_t>(CS::device_id::_MAX)) - item_resumed_total_amount;
    constexpr int32_t item_resumed_width_max = display_real_work_area[0];
    constexpr int32_t item_resumed_border_radius = 8;

    constexpr int32_t item_full_width_max = display_real_work_area[0];
    constexpr int32_t item_full_height_max = display_real_work_area[1];


    constexpr uint16_t item_font_color = 0;

    constexpr uint16_t item_online_bg_color = 0xc7b8;
    constexpr uint16_t item_offline_bg_color = 0xcf1e;

    constexpr uint16_t item_online_bg_color_border = 0x9e13;
    constexpr uint16_t item_offline_bg_color_border = 0xb69b;

    constexpr uint16_t item_has_issues_bg_color = 0xf638;
    constexpr uint16_t item_has_issues_bg_color_border = 0xb410;

    constexpr uint16_t item_close_button_x_and_border = 0xea8a;
    constexpr uint16_t item_close_button_body = 0x90c3;

    constexpr int32_t graph_margin_left = 70; // px
    constexpr int32_t graph_margin_right = 20; // px
    constexpr int32_t graph_margin_top = 80; // px
    constexpr int32_t graph_margin_bottom = 10; // px
    constexpr int32_t graph_width_calculated = display_real_work_area[0] - graph_margin_left - graph_margin_right;
    constexpr int32_t graph_height_calculated = display_real_work_area[1] - graph_margin_top - graph_margin_bottom;

    constexpr int32_t qrcode_margin_left = 80; // px
    constexpr int32_t qrcode_margin_top = 80; // px
    constexpr int32_t qrcode_size_pixel = 8; // px

    constexpr uint16_t graph_background_color = 0x9e13;
    constexpr uint16_t graph_line_color = 0xF800;

};

// ---- ---- END OF COREDISPLAY DEFAULTS BLOCK ---- ---- //

// ---- ---- LOGGER DEFAULTS BLOCK ---- ---- //

constexpr int logger_serial_speed = 19200; // in weird situations, may be used by SD card
constexpr size_t logger_buffer_len = 160;
constexpr char logger_log_path[] = "/log.txt";
constexpr char logger_exception_path[] = "/log_exceptions.txt";

// ---- ---- END OF LOGGER DEFAULTS BLOCK ---- ---- //

// ---- ---- SD CARD DEFAULTS BLOCK ---- ---- //

#define SPI_SDCARD VSPI
constexpr int sd_card_pins[] = {
    32, // SCK
    33, // MISO
    25, // MOSI
    26  // CS
};
constexpr uint64_t sd_check_sd_time_ms = 1000;
constexpr int sd_max_files_open = 2;
constexpr int sd_thread_priority = 17; // higher is more important.
constexpr uint32_t sd_max_timeout_wait_future = 10000; // ms
constexpr uint32_t sd_max_timeout_sd_card_full_of_tasks = 3500; // ms
constexpr size_t sd_max_tasks_pending = 6; // should be good for async events lmao

// ---- ---- END OF SD CARD DEFAULTS BLOCK ---- ---- //

// ---- ---- I2C CONTROLLER DEFAULTS BLOCK ---- ---- //

constexpr int i2c_pins[] = {
    4,  // SDA
    15  // SCL
};

constexpr size_t i2c_values_history_size = 20;
constexpr uint64_t i2c_path_checking_time = 30000; // ms
constexpr uint64_t i2c_packaging_delay = 5000; // ms
constexpr int i2c_thread_priority = tskIDLE_PRIORITY; // higher is more important.
constexpr bool wifi_hotspot_default = false;

// ---- ---- END OF I2C CONTROLLER DEFAULTS BLOCK ---- ---- //

// ---- ---- NON RELATED ---- ---- /

constexpr auto cpu_task_default_stack = 5120; 


// function to create threads easily
inline TaskHandle_t create_task(void(*fcn)(void*), const char* nam = "ASYNC", UBaseType_t pri = tskIDLE_PRIORITY, size_t stk = cpu_task_default_stack, void* arg = nullptr, int cid = -1) { TaskHandle_t _t = nullptr; if (cid < 0) { xTaskCreate(fcn, nam, stk, arg, pri, &_t); } else { xTaskCreatePinnedToCore(fcn, nam, stk, arg, pri, &_t, cid % portNUM_PROCESSORS); } return _t; }
// auto create task auto
#define acta(FUNCNAME) { create_task(FUNCNAME, #FUNCNAME); }
// auto create task priority
#define actp(FUNCNAME, PRIORITY) { create_task(FUNCNAME, #FUNCNAME, PRIORITY); }
// auto create task core
#define actc(FUNCNAME, COREID) { create_task(FUNCNAME, #FUNCNAME, tskIDLE_PRIORITY, cpu_task_default_stack, nullptr, COREID); }
// auto create task core prio
#define actcp(FUNCNAME, COREID, PRIORITY) { create_task(FUNCNAME, #FUNCNAME, PRIORITY, cpu_task_default_stack, nullptr, COREID); }
// auto create task core prio body
#define actcpb(BODY, COREID, PRIORITY) { create_task([](void*){ BODY; vTaskDelete(NULL); }, "ASYNCAUTO", PRIORITY, cpu_task_default_stack, nullptr, COREID); }
// auto create task core prio body argument
#define actcpba(BODY, COREID, PRIORITY, ARG) { create_task([](void* arg){ BODY; vTaskDelete(NULL); }, "ASYNCAUTO", PRIORITY, cpu_task_default_stack, ARG, COREID); }
// auto run class function as thread on core
#define async_class_method_pri(CLASSNAME, CLASSMETHOD, PRIORITY, COREID) create_task([](void* arg){ ((CLASSNAME*)arg)->CLASSMETHOD(); vTaskDelete(NULL); }, "CLASSASYNC", PRIORITY, cpu_task_default_stack, (void*)this, COREID);
#define async_class_method(CLASSNAME, CLASSMETHOD, COREID) async_class_method_pri(CLASSNAME, CLASSMETHOD, tskIDLE_PRIORITY, COREID)

#define MAKE_SINGLETON_CLASS_F(CLASSNAME, CLASSBODY, FUNCFLAGS) class CLASSNAME CLASSBODY; CLASSNAME& FUNCFLAGS get_singleton_of_##CLASSNAME(){ static CLASSNAME obj; return obj; }
#define MAKE_SINGLETON_CLASS_CF(CLASSNAME, CONSTRUCTOR, FUNCFLAGS) MAKE_SINGLETON_CLASS_F(CLASSNAME, { public: CLASSNAME() { CONSTRUCTOR; } }, FUNCFLAGS)
#define MAKE_SINGLETON_CLASS_INIT_F(CLASSNAME, CLASSBODY, FUNCFLAGS) MAKE_SINGLETON_CLASS_F(CLASSNAME, CLASSBODY, FUNCFLAGS) namespace ______dummy_namespace_for_##CLASSNAME{  const auto& ______dummy_ref_##CLASSNAME = get_singleton_of_##CLASSNAME(); }
#define MAKE_SINGLETON_CLASS_INIT_CF(CLASSNAME, CONSTRUCTOR, FUNCFLAGS) MAKE_SINGLETON_CLASS_CF(CLASSNAME, CONSTRUCTOR, FUNCFLAGS) namespace ______dummy_namespace_for_##CLASSNAME{  const auto& ______dummy_ref_##CLASSNAME = get_singleton_of_##CLASSNAME(); }

#define MAKE_SINGLETON_CLASS(CLASSNAME, CLASSBODY) class CLASSNAME CLASSBODY; CLASSNAME& get_singleton_of_##CLASSNAME(){ static CLASSNAME obj; return obj; }
#define MAKE_SINGLETON_CLASS_C(CLASSNAME, CONSTRUCTOR) MAKE_SINGLETON_CLASS(CLASSNAME, { public: CLASSNAME() { CONSTRUCTOR; } })
#define MAKE_SINGLETON_CLASS_INIT(CLASSNAME, CLASSBODY) MAKE_SINGLETON_CLASS(CLASSNAME, CLASSBODY) namespace ______dummy_namespace_for_##CLASSNAME{  const auto& ______dummy_ref_##CLASSNAME = get_singleton_of_##CLASSNAME(); }
#define MAKE_SINGLETON_CLASS_INIT_C(CLASSNAME, CONSTRUCTOR) MAKE_SINGLETON_CLASS_C(CLASSNAME, CONSTRUCTOR) namespace ______dummy_namespace_for_##CLASSNAME{  const auto& ______dummy_ref_##CLASSNAME = get_singleton_of_##CLASSNAME(); }

#define RUN_ONLY_ONCE(FUNCTIONNAME, ...) MAKE_SINGLETON_CLASS_INIT_C(RUNNABLE_##FUNCTIONNAME, FUNCTIONNAME(__VA_ARGS__), FUNCFLAGS );
#define RUN_ONLY_ONCE_FLAGGED(FUNCTIONNAME, FUNCFLAGS, ...) MAKE_SINGLETON_CLASS_INIT_C(RUNNABLE_##FUNCTIONNAME, FUNCTIONNAME(__VA_ARGS__), FUNCFLAGS );
#define RUN_ASYNC_ON_CORE_AUTO(CLASSNAME, THREADNAME, LOOPFUNC, COREID, PRIORITY) MAKE_SINGLETON_CLASS_INIT(ASYNC_##THREADNAME,  { CLASSNAME obj; public: ASYNC_##THREADNAME() { actcpba( while(1) { ((CLASSNAME*)arg)->LOOPFUNC(); yield(); }, COREID, PRIORITY, (void*)&obj); } const CLASSNAME& get_internal_variable() const { return obj; } CLASSNAME& get_internal_variable() { return obj; } }   ) CLASSNAME& get_singleton_of_##CLASSNAME() { return get_singleton_of_ASYNC_##THREADNAME().get_internal_variable(); }

#define GET(SINGLETON_NAME) get_singleton_of_##SINGLETON_NAME()

#define SLEEP(MILLISEC) { yield(); vTaskDelay(MILLISEC / portTICK_PERIOD_MS); }

//#define ASSERT_SD_LOCK_OVERWRITE(SDREF, PATH, BUFFER, LEN) for(size_t _e = 0; _e < LEN;) { const size_t _g = (_e == 0 ? SDREF.overwrite_on(PATH, BUFFER, LEN) : SDREF.append_on(PATH, BUFFER + _e, LEN - _e)); if (_g) {_e += _g; } else { Serial.printf("__INTERNAL: SDCARD LOCK MACRO GOT ZERO, DELAYED.\n"); SLEEP(1000); } }
//#define ASSERT_SD_LOCK_APPEND_ON(SDREF, PATH, BUFFER, LEN) for(size_t _e = 0; _e < LEN;) { const size_t _g = SDREF.append_on(PATH, BUFFER + _e, LEN - _e); if (_g) {_e += _g; } else { Serial.printf("__INTERNAL: SDCARD LOCK MACRO GOT ZERO, DELAYED.\n"); SLEEP(1000); } }

inline uint64_t get_time_ms() {return std::chrono::duration_cast<std::chrono::duration<uint64_t, std::milli>>(std::chrono::system_clock::now().time_since_epoch()).count(); }