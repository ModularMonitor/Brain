#pragma once

#include "CORE_Display.h"

#include "SD_card.h"
#include "Custom_Bitmaps.h"
#include "Configuration.h"

inline void CoreDisplay::task_touch()
{
    m_touch_history[1] = m_touch_history[0];

    if (m_touch_history[0].state = m_tft->getTouch(&m_touch_history[0].x, &m_touch_history[0].y))
    {
        m_screen_saver.next_step = get_time_ms() + GET(MyConfig).get_core_display_screen_saver_steps_time();
        if (m_screen_saver.state != 0) m_screen_saver.state = 3; // touch event on screen saver saves 3
    }

    if (!m_touch_history[0].state && m_touch_history[1].state) {// must be screen on!
        if (m_screen_saver.state == 0) _task_state_only_if_touch(m_touch_history[0]);
        else if (m_screen_saver.state > 2) m_screen_saver.state = 0; // reset history
    }
}

inline void CoreDisplay::task_display()
{
    //_task_update_screen_saver_state();
    _display_draw_bar_stuff(); // always











}

inline void CoreDisplay::_task_state_only_if_touch(const touch_event& event)
{
    using namespace DisplayColors;
    // do tasky tasks related to on_touch event

    LOGI(e_LOG_TAG::TAG_CORE, "Touch called.");

    const auto is_touch_on = [&](const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h) {
        const auto& m_now = m_touch_history[0];
        return m_now.x >= x && m_now.y >= y && m_now.x < (x + w) && m_now.y < (y + h);
    };

    uint16_t buttons{};
    for(size_t p = 0; p < 5; ++p) buttons |= (static_cast<uint16_t>(is_touch_on(480 - bar_right_width, bar_top_height + (p * bar_right_each_height), 40, 60)) << p);

    switch(m_state.state) {
    case core_states::STATE_HOME:
        // check for touch on devices
        break;
    case core_states::STATE_CONFIG:
        // touch on configurations
        break;
    case core_states::STATE_DETAILS:
        // no touch related stuff. Each one has its own max offset that each shows graph and current value
        break;
    case core_states::STATE_DEBUG:
        // on any touch on debug, go back
        m_state.state = core_states::STATE_HOME;
        m_state.offset = 0;
        m_state.offset_max = CS::d2u(CS::device_id::_MAX);
        break;
    }


    switch(buttons) {
    case 0x01: // home
        m_state.state = core_states::STATE_HOME;
        m_state.offset = 0;
        m_state.offset_max = CS::d2u(CS::device_id::_MAX);
        break;
    case 0x02: // up
        if (m_state.offset < m_state.offset_max) ++m_state.offset;
        break;
    case 0x04: // down
        if (m_state.offset > 0) --m_state.offset;
        break;
    case 0x08: // debug
        m_state.state = core_states::STATE_DEBUG;
        m_state.offset = 0;
        m_state.offset_max = 0;
        break;
    case 0x10: // config
        m_state.state = core_states::STATE_CONFIG;
        m_state.offset = 0;
        m_state.offset_max = 0;
        break;
    default: // if somehow
        return;
    }
}

inline void CoreDisplay::_task_calibrate_display_once()
{
    MySDcard& sd = GET(MySDcard);

    uint16_t calibrationData[6]{}; // 14 bytes actually

    m_tft->fillScreen(TFT_BLACK);
    m_tft->calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
    m_tft->fillScreen(TFT_BLACK);

    sd.async_overwrite_on(core_display_config_ini, (char*)calibrationData, 14);
}

inline void CoreDisplay::_display_draw_static_overlay()
{
    using namespace DisplayColors;

    m_tft->fillRect(0, 0, 480, bar_top_height, bar_top_color); // top bar bg
    m_tft->fillRect(480 - bar_right_width, bar_top_height, 480, 320, bar_right_color); // right bar menu
    m_tft->fillRect(0, 20, 440, 320, TFT_WHITE); // body


    m_tft->drawBitmap(442,  28, Bitmaps::config_icon_home,   40, 46, TFT_BLACK);
    m_tft->drawBitmap(447,  92, Bitmaps::config_icon_up,     26, 35, TFT_BLACK);
    m_tft->drawBitmap(447, 152, Bitmaps::config_icon_down,   26, 35, TFT_BLACK);
    m_tft->drawBitmap(444, 211, Bitmaps::config_icon_debug,  32, 38, TFT_BLACK);
    m_tft->drawBitmap(444, 274, Bitmaps::config_icon_config, 32, 32, TFT_BLACK);
}

inline void CoreDisplay::_display_draw_bar_stuff()
{
    using namespace DisplayColors;

    MySDcard& sd = GET(MySDcard);

    char buf[128];

    snprintf(buf, 128, "Time: %llu; SD usage: %.1f%%; CPU: %.1f%% %.1f%%; RAM %.1f%%",
        (get_time_ms() / 1000),
        sd.sd_used_perc() * 100.0f,
        CPU::get_cpu_usage(0) * 100.0f,
        CPU::get_cpu_usage(1) * 100.0f,
        CPU::get_ram_usage() * 100.0f
    );

    m_tft->setTextSize(1);
    m_tft->setTextColor(TFT_BLACK, bar_top_color); // top bar

    m_tft->drawString(buf, 2, 2, 2);
}


inline void CoreDisplay::async_display_screen_saver()
{
    LOGI(e_LOG_TAG::TAG_CORE, "Display screen brightness starting soon...");

    while(!m_is_tft_ready) SLEEP(100);

    LOGI(e_LOG_TAG::TAG_CORE, "Display screen brightness running!");

    uint8_t& curr_state = m_screen_saver.state;
    uint8_t& curr_level = m_screen_saver.current_level;
    uint64_t& time_event = m_screen_saver.next_step; // time for next bright down

    MyConfig& cfg = GET(MyConfig);

    while(1) {
        CPU::AutoWait autotime(core_display_led_pwn_delay); // loop control

        // if it's time, increase counter, lower screen brightness
        if (get_time_ms() > time_event) {
            time_event += cfg.get_core_display_screen_saver_steps_time();
            if (curr_state < 2) ++curr_state;
        }

        switch(curr_state) {
        case 1: // half
        {
            constexpr int target_val = 18;
            curr_level = static_cast<uint8_t>((static_cast<int>(curr_level) * 8 + target_val) / 9);
            if (abs(static_cast<int>(curr_level) - target_val) < 4) curr_level = target_val;
            analogWrite(core_display_led_pwm_pin, curr_level);
        }
            break;
        case 2: // off
        {
            constexpr int target_val = 0;
            curr_level = static_cast<uint8_t>((static_cast<int>(curr_level) * 8 + target_val) / 9);
            if (abs(static_cast<int>(curr_level) - target_val) < 4) curr_level = target_val;
            analogWrite(core_display_led_pwm_pin, curr_level);
        }
            break;
        default: // on
        {
            constexpr int target_val = 255;
            curr_level = static_cast<uint8_t>((static_cast<int>(curr_level) + target_val) / 2);
            if (abs(static_cast<int>(curr_level) - target_val) < 4) curr_level = target_val;
            analogWrite(core_display_led_pwm_pin, curr_level);
        }
            break;
        }
    }

    vTaskDelete(NULL);
}

inline void CoreDisplay::async_display_caller()
{
    MySDcard& sd = GET(MySDcard);

    LOGI(e_LOG_TAG::TAG_CORE, "Initializing Display...");

    m_tft = std::unique_ptr<TFT_eSPI>(new TFT_eSPI());
    
    LOGI(e_LOG_TAG::TAG_CORE, "Setting up Display...");

    m_tft->init();
    m_tft->setRotation(3);
    m_tft->fillScreen(TFT_BLACK);
    analogWrite(core_display_led_pwm_pin, 255);

    LOGI(e_LOG_TAG::TAG_CORE, "Getting Display calibration...");

    uint16_t calibrationData[6]{}; // 14 bytes actually + 1 extra byte for flag

    if (sd.read_from(core_display_config_ini, (char*)calibrationData, 14, 0) != 14) {
        LOGI(e_LOG_TAG::TAG_CORE, "No configuration has been found for touch calibration. Triggering one.");
        _task_calibrate_display_once();
        LOGI(e_LOG_TAG::TAG_CORE, "Calibration applied.");
    }
    else {
        LOGI(e_LOG_TAG::TAG_CORE, "Calibration good.");
        m_tft->setTouch(calibrationData);
    }
    
    LOGI(e_LOG_TAG::TAG_CORE, "Resetting timers...");

    m_screen_saver.next_step = get_time_ms() + GET(MyConfig).get_core_display_screen_saver_steps_time();

    LOGI(e_LOG_TAG::TAG_CORE, "Ready.");
    
    m_is_tft_ready = true;
    //m_tft->fillScreen(TFT_WHITE);
    _display_draw_static_overlay();

    while(1) {
        CPU::AutoWait autotime(core_display_main_delay); // loop control
        task_touch();

        if (m_screen_saver.state != 2) { // only update screen if makes sense
            task_display();
        }

        SLEEP(25);
    }
}

inline CoreDisplay::CoreDisplay()
{
    async_class_method_pri(CoreDisplay, async_display_caller, core_thread_priority, cpu_core_id_for_core);
    async_class_method_pri(CoreDisplay, async_display_screen_saver, core_led_pwm_thread_priority, cpu_core_id_for_display_pwm);

}