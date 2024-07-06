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
    STATE_DETAILS   // details of a module. offset is used as which variable is shown in graph
    //STATE_DEBUG     // print debug stuff
};

enum class core_settings_buttons {
    BTN_SCREEN_SAVER,               // change screen saver speed
    BTN_SAVE_SPEED,                 // change speed of i2c / store
    BTN_REDO_CALIBRATION_SCREEN,    // call calibration again
    _MAX
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
        unsigned offset_max = CS::d2u(CS::device_id::_MAX) - DisplayColors::item_resumed_amount_on_screen; // maximum value offset can have.
    };
private:
    touch_event m_touch_history[2];
    turn_off_screen_delayer m_screen_saver;
    state_control m_state;
    CS::device_id m_device_select = CS::device_id::DHT22_SENSOR; // "0"

    // drawing stuff
    std::unique_ptr<DisplayLineBlock[]> m_draw_lines;
    std::unique_ptr<DisplayFullBlockGraph> m_draw_full_graph;

    CPU::AutoWait m_animations_check_time{core_display_animations_check_time}; // checks paths periodically

    std::shared_ptr<TFT_eSPI> m_tft;
    bool m_is_tft_ready = false;

    // Task that checks for display finger release (causes touch event)
    void task_touch();
    // Task that shows information on 
    void task_display();

    // tool to check touch on xywz. Tests on last event ([0])
    bool __is_touch_on(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h);

    // This will be called if task_touch detects a touch event (by that)
    void _task_state_only_if_touch(const touch_event&);
    // This will be called periodically for the animations we all know (graphs update, flipping text) @ m_animations_check_time.
    void _task_update_animations();
    // Tool to calibrate and save Display calibration data on the SD card
    void _task_calibrate_display_once();
    // draw bars that should not be overlapped. Call only once.
    void _display_draw_static_overlay();
    // draw stats on top part
    void _display_draw_bar_stuff();
    // set all Display* objects to update screen
    void _set_all_state_has_changed();
    // Work on limits and data, generally called after touch event
    void _task_work_body_blocks_event();

    // Updates screen brightness in its own timing
    void async_display_screen_saver();
    // The async thread itself running the Display and states
    void async_display_caller();
public:
    CoreDisplay();
})

void ms2str(char* targ, const size_t lim, uint64_t to_c);