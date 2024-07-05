#pragma once

#include "CORE_Display.h"
#include "CORE_Display_aux_draw.ipp"

#include "Serial/packaging.h"
#include "Serial/flags.h"

#include "SD_card.h"
#include "Custom_Bitmaps.h"
#include "Configuration.h"


// from mseconds to days, print on targ
inline void ms2str(char* targ, const size_t lim, uint64_t to_c)
{
    constexpr uint64_t lim_millisec = 1000;
    constexpr uint64_t lim_seconds = 60 * lim_millisec;
    constexpr uint64_t lim_minutes = 60 * lim_seconds;
    constexpr uint64_t lim_hours = 24 * lim_minutes;
    // then days

    if (to_c == 0) {
        snprintf(targ, lim, "inf");
    }
    else if (to_c < lim_millisec) {
        snprintf(targ, lim, "%04llu ms", to_c);
    }
    else if (to_c < lim_seconds) {
        to_c /= lim_millisec; // sec
        snprintf(targ, lim, "%02llu seg", to_c);
    }
    else if (to_c < lim_minutes) {
        to_c /= lim_seconds; // min
        snprintf(targ, lim, "%02llu min", to_c);
    }
    else if (to_c < lim_hours) {
        to_c /= lim_minutes; // hour
        snprintf(targ, lim, "%02llu hora%s", to_c, to_c > 1 ? "s" : "");
    }
    else {
        to_c /= lim_hours; // day
        snprintf(targ, lim, "%02llu dia%s", to_c, to_c > 1 ? "s" : "");
    }
}


inline void CoreDisplay::task_touch()
{
    m_touch_history[1] = m_touch_history[0];

    if (m_touch_history[0].state = m_tft->getTouch(&m_touch_history[0].x, &m_touch_history[0].y))
    {
        m_screen_saver.next_step = get_time_ms();
        if (m_screen_saver.state != 0) m_screen_saver.state = 3; // touch event on screen saver saves 3
    }

    if (!m_touch_history[0].state && m_touch_history[1].state) {// must be screen on!
        if (m_screen_saver.state == 0) _task_state_only_if_touch(m_touch_history[0]);
        else if (m_screen_saver.state > 2) m_screen_saver.state = 0; // reset history
    }

    if (m_animations_check_time.is_time()) _task_update_animations();
}

inline void CoreDisplay::task_display()
{
    //_task_update_screen_saver_state();
    _display_draw_bar_stuff(); // always

    


    switch(m_state.state) {
    case core_states::STATE_HOME:
    case core_states::STATE_CONFIG:
    {
        for(size_t p = 0; p < DisplayColors::item_resumed_amount_on_screen; ++p) 
            m_draw_lines[p].draw(static_cast<int32_t>(p));
    }
        break;
    case core_states::STATE_DETAILS:
    {
        m_draw_full_graph->draw();
    }
        break;
    case core_states::STATE_DEBUG:
        break;
    }







}

inline bool CoreDisplay::__is_touch_on(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h) 
{
    const auto& m_now = m_touch_history[0];
    return m_now.x >= x && m_now.y >= y && m_now.x < (x + w) && m_now.y < (y + h);
}

inline void CoreDisplay::_task_state_only_if_touch(const touch_event& event)
{
    using namespace DisplayColors;
    // do tasky tasks related to on_touch event

    LOGI(e_LOG_TAG::TAG_CORE, "Touch called.");

    auto old_state = m_state.state;
        

    uint16_t buttons{};
    for(size_t p = 0; p < 5; ++p) buttons |= (static_cast<uint16_t>(__is_touch_on(480 - bar_right_width, bar_top_height + (p * bar_right_each_height), 40, 60)) << p);

    // = = = = = TEST BODY BLOCKS = = = = = //

    _task_work_body_blocks_event();

    // = = = = = TEST LATERAL BUTTONS = = = = = //

    switch(buttons) {
    case 0x01: // home
        m_state.state = core_states::STATE_HOME;
        m_state.offset = 0;
        m_state.offset_max = CS::d2u(CS::device_id::_MAX) - item_resumed_amount_on_screen;
        break;
    case 0x02: // up
        if (m_state.offset > 0) --m_state.offset;
        break;
    case 0x04: // down
        if (m_state.offset < m_state.offset_max) ++m_state.offset;
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
        break;
    }

    // just check just in case...
    if (m_state.offset > m_state.offset_max) m_state.offset = m_state.offset_max;
    if (old_state != m_state.state) _set_all_state_has_changed();

    LOGI_NOSD(e_LOG_TAG::TAG_CORE, "Var %u", m_state.offset);

    _task_update_animations(); // update as soon as possible on touch
}

// == == == == == == UPDATE ITEMS ON SCREEN TO DRAW LATER! == == == == == == //
inline void CoreDisplay::_task_update_animations()
{
    using namespace DisplayColors;

    switch(m_state.state) {
    case core_states::STATE_HOME:
    {
        const auto& off_now = m_state.offset;
        const MyI2Ccomm& com = GET(MyI2Ccomm);

        for (size_t p = 0; p < item_resumed_amount_on_screen; ++p) 
        {
            const CS::device_id real_off = static_cast<CS::device_id>((p + static_cast<size_t>(m_state.offset)) % static_cast<size_t>(CS::d2u(CS::device_id::_MAX))); // just to be sure 100%
            const MyI2Ccomm::device& dev = com.get_device_configurations(real_off, 0); // real_off cannot be bigger than expected already


            if (dev.m_map.size() != 0) {
                std::lock_guard<std::mutex> l(dev.m_map_mtx); // security first
                const unsigned sel = get_time_ms() % dev.m_map.size();

                const i2c_data_pair& sel_pair = *std::next(dev.m_map.begin(), sel);
                const std::string tmp = sel_pair.first + ": " + sel_pair.second;
                const uint64_t time_since = get_time_ms() - dev.m_update_time;
                char buf_time_since[64];
                ms2str(buf_time_since, sizeof(buf_time_since), time_since);
                const std::string tmp2 = "Atualizado faz: " + std::string(buf_time_since);
                
                m_draw_lines[p].set_texts(
                    get_fancy_name_for(real_off),
                    tmp.c_str(),
                    tmp2.c_str()
                );
            }
            else {
                m_draw_lines[p].set_texts(
                    get_fancy_name_for(real_off),
                    "Sem dados ainda",
                    ""
                );
            }
        }
    }
        break;
    case core_states::STATE_CONFIG:
    {
        char buf_time_since[64];
        for (size_t p = 0; p < item_resumed_amount_on_screen; ++p) 
        {
            switch(p) {
            case static_cast<size_t>(core_settings_buttons::BTN_SCREEN_SAVER):
                ms2str(buf_time_since, sizeof(buf_time_since), GET(MyConfig).get_core_display_screen_saver_steps_time());

                m_draw_lines[p].set_texts(
                    "Tempo de desligamento de tela:",
                    buf_time_since,
                    ""
                );
                break;
            case static_cast<size_t>(core_settings_buttons::BTN_SAVE_SPEED):
                ms2str(buf_time_since, sizeof(buf_time_since), GET(MyConfig).get_i2c_packaging_delay());
                
                m_draw_lines[p].set_texts(
                    "Tempo de dados dos dispositivos:",
                    buf_time_since,
                    ""
                );
                break;
            default:
                m_draw_lines[p].set_texts("","","");
                break;
            }
        }        
    }
        break;
    case core_states::STATE_DETAILS:
    {
        m_state.offset_max = GET(MyI2Ccomm).get_device_configurations(m_device_select, 0).m_map.size();


    }
        break;
    case core_states::STATE_DEBUG:
        break;
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
    m_tft->fillRect(0, 20, 440, 320, body_color); // body


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

    // What do I want on my top bar?
    /*
    1. SD card usage / online
    2. Devices connected
    3. I2C time (delay)
    */


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

inline void CoreDisplay::_set_all_state_has_changed()
{
    for(size_t p = 0; p < DisplayColors::item_resumed_amount_on_screen; ++p) m_draw_lines[p].set_state_changed();
    m_draw_full_graph->set_state_changed();
}

inline void CoreDisplay::_task_work_body_blocks_event()
{
    using namespace DisplayColors;

    switch(m_state.state) {
    case core_states::STATE_HOME:
        // check for touch on devices
    {
        for (size_t p = 0; p < item_resumed_amount_on_screen; ++p)
        {
            const int32_t begin_top = bar_top_height + p * item_resumed_height_max;

            if (__is_touch_on(0, begin_top, item_resumed_width_max, item_resumed_height_max)) {
                size_t calc = p + m_state.offset;

                LOGI_NOSD(e_LOG_TAG::TAG_CORE, "Calc %zu", calc);

                if (calc >= static_cast<size_t>(CS::d2u(CS::device_id::_MAX))) m_device_select = CS::device_id::DHT22_SENSOR;
                else m_device_select = static_cast<CS::device_id>(calc);

                m_state.state = core_states::STATE_DETAILS;
                m_state.offset = 0;
                m_state.offset_max = GET(MyI2Ccomm).get_device_configurations(m_device_select, 0).m_map.size();

                m_draw_full_graph->update_with(m_device_select, m_state.offset);
                break;
            }
        }
//        const auto& off_now = m_state.offset;
//        const MyI2Ccomm& com = GET(MyI2Ccomm);
//
//        for (size_t p = 0; p < item_resumed_amount_on_screen; ++p) 
//        {
//            const CS::device_id real_off = static_cast<CS::device_id>((p + static_cast<size_t>(m_state.offset)) % static_cast<size_t>(CS::d2u(CS::device_id::_MAX) - 1)); // just to be sure 100%
//            //const MyI2Ccomm::device& dev = com.get_device_configurations(static_cast<CS::device_id>(real_off), 0); // real_off cannot be bigger than expected already
//            const char* name = get_fancy_name_for(real_off);
//
//            m_draw_lines[p].set_texts(name, "Loading...", "        ");
//        }
    }
        break;
    case core_states::STATE_CONFIG:
        // touch on configurations
    {
        for (size_t p = 0; p < item_resumed_amount_on_screen; ++p)
        {
            const int32_t begin_top = bar_top_height + p * item_resumed_height_max;

            if (__is_touch_on(0, begin_top, item_resumed_width_max, item_resumed_height_max)) {
                const size_t calc = p + m_state.offset;

                LOGI_NOSD(e_LOG_TAG::TAG_CORE, "Calc %zu", calc);

                MyConfig& cfg = GET(MyConfig);

                if (calc >= static_cast<size_t>(core_settings_buttons::_MAX)) break;
                
                switch(static_cast<core_settings_buttons>(calc)){
                case core_settings_buttons::BTN_SCREEN_SAVER:
                {
                    switch(cfg.get_core_display_screen_saver_steps_time()) {
                    case 0:
                        cfg.set_core_display_screen_saver_steps_time(10000);
                        //m_draw_lines[p].set_texts(nullptr, nullptr, "10 seconds");
                        break;
                    case 10000:
                        cfg.set_core_display_screen_saver_steps_time(20000);
                        //m_draw_lines[p].set_texts(nullptr, nullptr, "20 seconds");
                        break;
                    case 20000:
                        cfg.set_core_display_screen_saver_steps_time(30000);
                        //m_draw_lines[p].set_texts(nullptr, nullptr, "30 seconds");
                        break;
                    case 30000:
                        cfg.set_core_display_screen_saver_steps_time(45000);
                        //m_draw_lines[p].set_texts(nullptr, nullptr, "45 seconds");
                        break;
                    case 45000:
                        cfg.set_core_display_screen_saver_steps_time(60000);
                        //m_draw_lines[p].set_texts(nullptr, nullptr, "1 minute");
                        break;
                    case 60000:
                        cfg.set_core_display_screen_saver_steps_time(120000);
                        //m_draw_lines[p].set_texts(nullptr, nullptr, "2 minutes");
                        break;
                    case 120000:
                        cfg.set_core_display_screen_saver_steps_time(300000);
                        //m_draw_lines[p].set_texts(nullptr, nullptr, "5 minutes");
                        break;
                    case 300000:
                        cfg.set_core_display_screen_saver_steps_time(600000);
                        //m_draw_lines[p].set_texts(nullptr, nullptr, "10 minutes");
                        break;
                    case 600000:
                        cfg.set_core_display_screen_saver_steps_time(0);
                        //m_draw_lines[p].set_texts(nullptr, nullptr, "infinite");
                        break;
                    default:
                        cfg.set_core_display_screen_saver_steps_time(10000);
                        break;
                    }
                }
                    break;
                case core_settings_buttons::BTN_SAVE_SPEED:
                {
                    switch(cfg.get_i2c_packaging_delay()) {
                    case 1000:
                        cfg.set_i2c_packaging_delay(2000); // 2 s
                        break;
                    case 2000:
                        cfg.set_i2c_packaging_delay(3000); // 3 s
                        break;
                    case 3000:
                        cfg.set_i2c_packaging_delay(4000); // 4 s
                        break;
                    case 4000:
                        cfg.set_i2c_packaging_delay(5000); // 5 s
                        break;
                    case 5000:
                        cfg.set_i2c_packaging_delay(7500); // 7.5 s
                        break;
                    case 7500:
                        cfg.set_i2c_packaging_delay(10000); // 10 s
                        break;
                    case 10000:
                        cfg.set_i2c_packaging_delay(15000); // 15 s
                        break;
                    case 15000:
                        cfg.set_i2c_packaging_delay(20000); // 20 s
                        break;
                    case 20000:
                        cfg.set_i2c_packaging_delay(30000); // 30 s
                        break;
                    case 30000:
                        cfg.set_i2c_packaging_delay(45000); // 45 s
                        break;
                    case 45000:
                        cfg.set_i2c_packaging_delay(60000); // 1 min
                        break;
                    case 60000:
                        cfg.set_i2c_packaging_delay(90000); // 1.5 min
                        break;
                    case 90000:
                        cfg.set_i2c_packaging_delay(120000); // 2 min
                        break;
                    case 120000:
                        cfg.set_i2c_packaging_delay(180000); // 3 min
                        break;
                    case 180000:
                        cfg.set_i2c_packaging_delay(300000); // 5 min
                        break;
                    case 300000:
                        cfg.set_i2c_packaging_delay(600000); // 10 min
                        break;
                    case 600000:
                        cfg.set_i2c_packaging_delay(1200000); // 20 min
                        break;
                    case 1200000:
                        cfg.set_i2c_packaging_delay(1800000); // 30 min
                        break;
                    case 1800000:
                        cfg.set_i2c_packaging_delay(3600000); // 1 h
                        break;
                    case 3600000:
                        cfg.set_i2c_packaging_delay(7200000); // 2 h
                        break;
                    case 7200000:
                        cfg.set_i2c_packaging_delay(10800000); // 3 h
                        break;
                    case 10800000:
                        cfg.set_i2c_packaging_delay(21600000); // 6 h
                        break;
                    case 21600000:
                        cfg.set_i2c_packaging_delay(43200000); // 12 h
                        break;
                    case 43200000:
                        cfg.set_i2c_packaging_delay(86400000); // 24 h
                        break;
                    default:
                        cfg.set_i2c_packaging_delay(1000);
                        break;
                    }
                }
                    break;
                default:
                    break;
                }
                break; // of if
            }

        }        
    }
        break;
    case core_states::STATE_DETAILS:
        // no touch related stuff. Each one has its own max offset that each shows graph and current value
        // on touch is should do nothing actually.
    {
        //m_state.offset_max = GET(MyI2Ccomm).get_device_configurations(m_device_select, 0).m_map.size();
    }
        break;
    case core_states::STATE_DEBUG:
        // on any touch on debug, go back
        m_state.state = core_states::STATE_HOME;
        m_state.offset = 0;
        m_state.offset_max = CS::d2u(CS::device_id::_MAX) - item_resumed_amount_on_screen;
        break;
    }
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

        // if it's time, increase counter, lower screen brightness. If 0, keep on.
        if (cfg.get_core_display_screen_saver_steps_time() == 0) {
            curr_state = 0;
            time_event = 0;
        }
        else if (get_time_ms() > time_event + GET(MyConfig).get_core_display_screen_saver_steps_time()) {
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

    m_tft = std::shared_ptr<TFT_eSPI>(new TFT_eSPI());
    
    m_draw_lines = std::unique_ptr<DisplayLineBlock[]>(new DisplayLineBlock[DisplayColors::item_resumed_amount_on_screen]);
    for(size_t p = 0; p < DisplayColors::item_resumed_amount_on_screen; ++p) {
        m_draw_lines[p].set_tft(m_tft);
        m_draw_lines[p].set_fill_color(DisplayColors::item_offline_bg_color);
        m_draw_lines[p].set_border_color(DisplayColors::item_offline_bg_color_border);
        m_draw_lines[p].set_font_color(DisplayColors::item_font_color);
        m_draw_lines[p].set_nodata_color(DisplayColors::body_color);
    }

    m_draw_full_graph = std::unique_ptr<DisplayFullBlockGraph>(new DisplayFullBlockGraph{});
    {
        m_draw_full_graph->set_tft(m_tft);
        m_draw_full_graph->set_fill_color(DisplayColors::item_offline_bg_color);
        m_draw_full_graph->set_border_color(DisplayColors::item_offline_bg_color_border);
        m_draw_full_graph->set_font_color(DisplayColors::item_font_color);
        m_draw_full_graph->set_nodata_color(DisplayColors::body_color);
    }

    LOGI(e_LOG_TAG::TAG_CORE, "Setting up Display...");

    m_tft->init();
    m_tft->setRotation(3);
    m_tft->fillScreen(TFT_BLACK);
    analogWrite(core_display_led_pwm_pin, 255);

    LOGI(e_LOG_TAG::TAG_CORE, "Getting Display calibration...");
    SLEEP(25);

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

    m_screen_saver.next_step = get_time_ms();

    LOGI(e_LOG_TAG::TAG_CORE, "Ready.");
    
    m_is_tft_ready = true;
    //m_tft->fillScreen(body_color);
    _display_draw_static_overlay();
    //_task_state_only_if_touch(m_touch_history[0]); // trigger to setup variables

    SLEEP(25);
    
    while(1) {
        CPU::AutoWait autotime(core_display_main_delay); // loop control
        task_touch();
        SLEEP(15);

        if (m_screen_saver.state != 2) { // only update screen if makes sense
            task_display();
        }

        SLEEP(15);
    }
}

inline CoreDisplay::CoreDisplay()
{
    async_class_method_pri(CoreDisplay, async_display_caller, core_thread_priority, cpu_core_id_for_core);
    async_class_method_pri(CoreDisplay, async_display_screen_saver, core_led_pwm_thread_priority, cpu_core_id_for_display_pwm);

}