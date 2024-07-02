/*
SD_card.h

# Depends on:
- (everything)

# Description:
- This header contains the implementation of the synchronous SD card class "MySDcard", which is a singleton
and must be only referenced using the appropriate GET(MySDcard) as defaults.h defines it.
- This is an sync wrapper on the SD library from Arduino, with pins set from defaults.h, that allows any thread
to call SD functions.

*/

#pragma once

#include "defaults.h"

#include "CPU_control.h"
#include "LOG_ctl.h"
#include "I2C_communication.h"

#include "FS.h"
#include "SPI.h"
#include "TFT/TFT_eSPI.h"
#include "free_fonts.h"

enum class core_states {
    STATE_HOME,     // default view, shows every module, has offset
    STATE_CONFIG,   // config allows for recalibration of screen and period of reporting on SD card
    STATE_DETAILS,  // details of a module. offset is used as which variable is shown in graph
    STATE_DEBUG     // print debug stuff
};

namespace DisplayColors {
    constexpr size_t log_line_dist = 20;
    constexpr size_t log_amount = 320 / log_line_dist;
    constexpr size_t log_line_max_len = 96;

    constexpr int32_t bar_top_height = 20;
    constexpr int32_t bar_right_width = 40;
    constexpr int32_t bar_right_each_height = (320 - bar_top_height) / 5;

    constexpr uint16_t bar_top_color = 0x34da;
    constexpr uint16_t bar_right_color = 0xcd49;

    constexpr int32_t item_resumed_height_max = 60;
    constexpr int32_t item_resumed_total_amount = (320 - bar_top_height) / item_resumed_height_max;
    constexpr int32_t item_resumed_max_offset = (static_cast<int32_t>(CS::device_id::_MAX)) - item_resumed_total_amount;
    constexpr int32_t item_resumed_width_max = 440;
    constexpr int32_t item_resumed_border_radius = 8;

    const uint16_t item_online_bg_color = 0xc7b8;
    const uint16_t item_offline_bg_color = 0xcf1e;
    const uint16_t item_online_bg_color_border = 0x9e13;
    const uint16_t item_offline_bg_color_border = 0xb69b;
    const uint16_t item_has_issues_bg_color = 0xf638;
    const uint16_t item_has_issues_bg_color_border = 0xb410;

    const uint16_t item_close_button_x_and_border = 0xea8a;
    const uint16_t item_close_button_body = 0x90c3;
};

// As of a core itself, it doesn't have public calls. It does by itself everything. This is the CORE. This rules everything
MAKE_SINGLETON_CLASS(CoreDisplay, {
public:
    struct touch_event {
        uint16_t x{};
        uint16_t y{};
        bool state = false;
    };
    struct turn_off_screen_delayer {
        uint8_t state = 0; // 0 == full brightness, 1 == half, 2 == off. On non full brightness, ignore touch, only bright up.
        uint8_t current_level = 255;
        uint64_t next_step = core_display_screen_saver_steps_time;
    };
    struct state_control {
        core_states state = core_states::STATE_HOME;
        unsigned offset = 0; // up or down, reset on change
        unsigned offset_max = 0; // maximum value offset can have.
        uint8_t selected_module = 0; // only useful on STATE_DETAILS
    };
private:
    touch_event m_touch_history[2];
    turn_off_screen_delayer m_screen_saver;
    state_control m_state;
    

    std::shared_ptr<TFT_eSPI> m_tft;
    bool m_is_tft_ready = false;

    // Task that checks for display finger release (causes touch event)
    void task_touch();
    // Task that shows information on 
    void task_display();

    // This will be called if task_touch detects a touch event (by that)
    void _task_state_only_if_touch(const touch_event&);
    // Tool to calibrate and save Display calibration data on the SD card
    void _task_calibrate_display_once();
    // draw bars that should not be overlapped. Call only once.
    void _display_draw_static_overlay();
    // draw stats on top part
    void _display_draw_bar_stuff();

    // Updates screen brightness in its own timing
    void async_display_screen_saver();
    // The async thread itself running the Display and states
    void async_display_caller();
public:
    CoreDisplay();
})