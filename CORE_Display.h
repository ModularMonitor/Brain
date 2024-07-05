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
#include "CORE_Display_aux_draw.h"

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
    CS::device_id m_device_select = CS::device_id::DHT22_SENSOR; // "0"

    // drawing stuff
    std::unique_ptr<DisplayLineBlock[]> m_draw_lines;
    std::unique_ptr<DisplayFullBlockGraph> m_draw_full_graph;

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