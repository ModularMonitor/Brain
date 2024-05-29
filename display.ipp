#pragma once
#include "display.h"

#include "common.h"
#include "cpu_ctl.h"
#include "sdcard.h"

namespace DP {

    namespace Bitmaps {
        // 11 x 13 SD card icon:
        PROGMEM const unsigned char sd_card_icon_11_13[] = {
            0b00011111, 0b11100000,
            0b00111111, 0b11100000,
            0b01111111, 0b11100000,
            0b11111000, 0b11100000,
            0b11110000, 0b11100000,
            0b11100000, 0b11100000,
            0b11100000, 0b11100000,
            0b11100000, 0b11100000,
            0b11100000, 0b11100000,
            0b11100000, 0b11100000,
            0b11111111, 0b11100000,
            0b11111111, 0b11100000,
            0b11111111, 0b11100000
        };

        // 11 x 14 DB icon
        PROGMEM const unsigned char database_icon_11_14[] = {
            0b01111111, 0b11000000,
            0b11111111, 0b11100000,
            0b11100000, 0b11100000,
            0b11111111, 0b11100000,
            0b11111111, 0b11100000,
            0b11011111, 0b01100000,
            0b11000000, 0b01100000,
            0b11000000, 0b01100000,
            0b11000000, 0b01100000,
            0b11000000, 0b01100000,
            0b11100000, 0b11100000,
            0b11111111, 0b11100000,
            0b01111111, 0b11000000,
            0b00111111, 0b10000000
            
        };

    }

    inline Display::Display()
        : m_tft(std::unique_ptr<TFT_eSPI>(new TFT_eSPI()))
    {
        m_tft->init();
        m_tft->setRotation(3);
        m_tft->fillScreen(TFT_DARKGREEN);
        SET_DISPLAY_BRIGHTNESS(0.2f);

        //m_tft->setTextColor(TFT_WHITE, TFT_BLACK);
        //m_tft->setTextSize(1);
        //m_tft->setCursor(0, 0, 2);
        
        LOGI(TAG, "Display launched!");
        this->terminal_print();
    }

    inline TFT_eSPI* Display::operator->()
    {
        return m_tft.get();
    }

    inline const TFT_eSPI* Display::operator->() const
    {
        return m_tft.get();
    }

    inline void Display::terminal_print()
    {
        //m_tft->fillScreen(TFT_BLACK);
        m_tft->setTextColor(TFT_WHITE, TFT_BLUE);
        m_tft->setTextSize(1);

        const auto& logg = ::LG::get_singleton_of_Logger();

        for(size_t pp = 0; pp < logg.size(); ++pp) {
            const size_t p = logg.size() - pp - 1;

            const char* line = logg.get_line(p);

            int16_t off_txt = m_tft->textWidth(line, 2);
            if (off_txt < 480) m_tft->fillRect(off_txt, 320 - ::LG::log_line_dist * (p+1), 480 - off_txt, ::LG::log_line_dist + 1, TFT_DARKGREEN);

            //m_tft->setCursor(0, 320 - ::LG::log_line_dist * (p+1), 2);
            m_tft->drawString(line, 0, 320 - ::LG::log_line_dist * (p+1), 2); // , ::LG::log_line_max_len
        }
    }

    inline std::shared_ptr<TFT_eSPI> Display::share_tft() const
    {
        return m_tft;
    }


    inline TouchCtl::TouchCtl(Display& disp)
        : m_disp(disp)
    {
        pinMode(TOUCH_CS, OUTPUT);

        LOGI(TAG, "=== TouchCtl block ===");
        LOGI(TAG, "Checking file " DISPLAY_CALIBRATION_FILE "...");
        m_disp.terminal_print();

        uint16_t calibrationData[6]{}; // 14 bytes actually + 1 extra byte for flag

        if (!SDcard::is_sd_init()) {
            LOGE(TAG, "SD card is not present! Touch calibration will run all times this way! Please insert a SD Card next time.");

            m_disp.terminal_print();
            delay(1000);
            
            m_disp->fillScreen(TFT_BLACK);
            m_disp->calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
            m_disp->fillScreen(TFT_BLACK);

            m_disp.terminal_print();
        }
        else { // has SD card

            CPU::run_on_core_sync([](void* a){
                uint16_t* calData = (uint16_t*)a;

                File f = SDcard::f_open(DISPLAY_CALIBRATION_FILE, "r");

                if (!f || (f.readBytes((char*)calData, 14) != 14)) {
                    LOGI(TAG, "File " DISPLAY_CALIBRATION_FILE " not found or empty.");
                    calData[5] = 2;
                }
                else {
                    LOGI(TAG, "File " DISPLAY_CALIBRATION_FILE " found and ready to set!");
                    calData[5] = 1;
                }
                f.close();
            }, cpu_core_id_for_sd_card, (void*)calibrationData);

            m_disp.terminal_print();

            switch(calibrationData[5]) {
            case 1:
            {
                LOGI(TAG, "Found display calibration. Applying... ");
                m_disp->setTouch(calibrationData);
                LOGI(TAG, "File " DISPLAY_CALIBRATION_FILE " applied!");
                m_disp.terminal_print();
            }
                break;
            case 2:
            {
                LOGI(TAG, "No file or data found, calibration requested!");

                m_disp.terminal_print();

                delay(1000);
                
                m_disp->fillScreen(TFT_BLACK);
                m_disp->calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
                m_disp->fillScreen(TFT_BLACK);

                CPU::run_on_core_sync([](void* a){
                    uint16_t* calData = (uint16_t*)a;

                    File f = SDcard::f_open(DISPLAY_CALIBRATION_FILE, "w");

                    if (!f) {
                        LOGI(TAG, "Could not create/open " DISPLAY_CALIBRATION_FILE " file to write. Save failed.");
                    }

                    f.write((uint8_t*)calData, 14);
                    f.close();

                    LOGI(TAG, "Wrote " DISPLAY_CALIBRATION_FILE " successfully.");
                }, cpu_core_id_for_sd_card, (void*)calibrationData);

                m_disp.terminal_print();
            }
                break;
            default:
                LOGE(TAG, "Undefined behaviour on Display::Display. Skipping calibration.");
                m_disp.terminal_print();
                break;
            }
        }

        LOGI(TAG, "TouchCtl is ready and running.");
        LOGI(TAG, "=== TouchCtl block ===");

        m_disp.terminal_print();
    }

    inline bool TouchCtl::task()
    {
        m_b4 = m_now;
        m_now.state = m_disp->getTouch(&m_now.x, &m_now.y);

        if (m_b4.state && !m_now.state)
        {
            m_now.last_switch_false = CPU::get_time_ms();
            return true;
        }
        return false;
    }

    inline uint16_t TouchCtl::get_x(bool old) const
    {
        return old ? m_b4.x : m_now.x;
    }

    inline uint16_t TouchCtl::get_y(bool old) const
    {
        return old ? m_b4.y : m_now.y;
    }

    inline uint64_t TouchCtl::get_time_ms(bool old) const
    {
        return old ? m_b4.last_switch_false : m_now.last_switch_false;
    }

    inline uint64_t TouchCtl::last_event_was_ms() const
    {
        return CPU::get_time_ms() - m_now.last_switch_false;
    }

    inline int32_t TouchCtl::get_dx() const
    {
        return static_cast<int32_t>(m_now.x) - static_cast<int32_t>(m_b4.x);
    }

    inline int32_t TouchCtl::get_dy() const
    {
        return static_cast<int32_t>(m_now.y) - static_cast<int32_t>(m_b4.y);
    }

    inline uint64_t TouchCtl::get_delta_time_of_last_ms() const
    {
        return m_now.last_switch_false - m_b4.last_switch_false;
    }

    inline bool TouchCtl::is_down() const
    {
        return m_now.state;
    }

    inline bool TouchCtl::is_touch_on(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h) const
    {
        return m_now.x >= x && m_now.y >= y && m_now.x < (x + w) && m_now.y < (y + h);
    }


    inline void DisplayCtl::draw_mouse(bool bg) 
    {
        const int32_t nx = m_touch->get_x();
        const int32_t ny = m_touch->get_y();
        const int32_t ox = m_touch->get_x(true);
        const int32_t oy = m_touch->get_y(true);

        const auto draw_arrow = [&](const int32_t x, const int32_t y, const int color) {
            m_tft->drawFastHLine(x, y, 6, color);
            m_tft->drawFastHLine(x, y + 1, 6, color);
            m_tft->drawFastVLine(x, y, 6, color);
            m_tft->drawFastVLine(x + 1, y, 6, color);
            
            m_tft->drawLine(x, y, x + 6, y + 6, color);
            m_tft->drawLine(x + 1, y, x + 7, y + 6, color);
            m_tft->drawLine(x, y + 1, x + 6, y + 7, color);
        };

        if (bg) draw_arrow(ox, oy, TFT_BLACK);
        draw_arrow(nx, ny, TFT_WHITE);
    }

    inline void DisplayCtl::task()
    {
        // track screen change
        screen next_screen = m_screen;

        // Guarantee everything is loaded
        if (!m_disp) {
            m_disp = new Display();
            m_touch = new TouchCtl(*m_disp);
            m_tft = m_disp->share_tft();

            LOGI(TAG, "Loaded all modules. Ready to start UI...");
            m_disp->terminal_print();

            delay(500);
            SET_DISPLAY_BRIGHTNESS(0.5f);
            delay(500);
            SET_DISPLAY_BRIGHTNESS(1.0f);

            next_screen = screen::HOME;
        }

        // logic of stuff going on

        // ======== ALWAYS RUN BLOCK ======== //
        const bool had_touch_event = m_touch->task();
        //if (m_touch->task()) { // touch event, task.
        //    LOGI(TAG, "Touch event @ %i:%i (delta: %" PRIu64 " ms)", (int)m_touch->get_x(), (int)m_touch->get_y(), m_touch->get_delta_time_of_last_ms());
        //}

        // ======== END OF ALWAYS RUN BLOCK ======== //

        // ================ THINK BLOCK ================ //
        if (had_touch_event) {
            switch(next_screen) {
            case screen::DEBUG_CMD: // only log screen
            {
                if (m_touch->is_touch_on(460, 0, 20, 20))
                    next_screen = screen::HOME;
            }
                break;
            case screen::HOME:
            {
                //if (m_touch->is_touch_on(460, 0, 20, 20))
                //    next_screen = screen::DEBUG_CMD;
            }
                break;
            }
        }
        if (m_ext_cmd_req) { // button pressed, things like that
            m_ext_cmd_req = false;
            next_screen = screen::DEBUG_CMD;
        }
        // ================ END OF THINK BLOCK ================ //

        // ================ DRAW BLOCK ================ //

        // TOOLS:
        const bool state_changed = next_screen != m_screen;
        STR::SharedData& shared_data = STR::get_singleton_of_SharedData();

        switch(next_screen) {
        case screen::DEBUG_CMD: // only log screen
        {
            const bool update_forced = (!m_touch->is_down() && m_touch->last_event_was_ms() < 400);

            if (state_changed) m_tft->fillScreen(TFT_DARKGREEN);

            // if had new line on log or touch stuff
            if (LG::get_singleton_of_Logger().for_display_had_news() || update_forced || state_changed) {
                m_disp->terminal_print();
                m_tft->fillRect(460, 0, 20, 20, TFT_RED);
                m_tft->drawLine(462, 2, 478, 18, 0xA000);
                m_tft->drawLine(478, 18, 462, 2, 0xA000);
            }

            // touch shenanigans
            if (m_touch->is_down() || update_forced) {
                draw_mouse();
            }
        }
            break;
        case screen::HOME:
        {
            STR::SIMData& sim = shared_data.get_sim_data();

            m_tft->setTextSize(1);

            if (state_changed) {
                m_tft->fillRect(0, 0, 480, 20, 0x34da); // top bar bg
                m_tft->fillRect(440, 20, 480, 320, 0xcd49); // right bar menu
                m_tft->fillRect(0, 20, 440, 320, TFT_WHITE); // body

                m_tft->setTextColor(TFT_BLACK, 0x34da); // top bar

                m_tft->drawString("Waiting 4G...", 2, 2, 2);
                m_tft->drawString("...", 457, 2, 2);
                m_tft->drawString("4G", 406, 2, 2);

                m_tft->drawBitmap(376, 4, Bitmaps::sd_card_icon_11_13, 11, 13, TFT_BLACK);
                m_tft->drawBitmap(289, 3, Bitmaps::database_icon_11_14, 11, 14, TFT_BLACK);
            }
            else {
                m_tft->setTextColor(TFT_BLACK, 0x34da); // top bar

                // = = = = = = TIME (clock) = = = = = = //
                if (sim.has_new_data_of(STR::SIMData::test_has_new_data_of::TIME) || (m_clock_update_time.is_time() && sim.get_rssi() != -1)) {
                    char safe_buffer[96];
                    m_tft->drawString(sim.get_time(safe_buffer, 96), 2, 2, 2);
                }

                // = = = = = = SD Card status = = = = = = //
                if (m_sdcard_update_time.is_time()) {
                    const char* stat = SDcard::sd_get_type();
                    const int wd = m_tft->textWidth(stat, 2);

                    m_tft->fillRect(304, 2, 70, 16, 0x34da); // fill color
                    m_tft->drawString(stat, 374 - wd - 2, 2, 2);
                }

                // = = = = = = DB connection = = = = = = //
                
                // = = = = = = RSSI (bar and text) = = = = = = //
                if (sim.get_rssi() == -1) {
                    m_tft->drawString("??", 457, 2, 2);

                    switch((CPU::get_time_ms() / 200) % 4){
                    case 3:
                        m_tft->fillRect(448,  3, 5, 14, TFT_BLACK);
                        m_tft->fillRect(441,  6, 5, 11, 0x34da);
                        //m_tft->fillRect(434,  9, 5,  8, 0x34da);
                        //m_tft->fillRect(427, 12, 5,  5, 0x34da);
                        break;
                    case 2:
                        //m_tft->fillRect(448,  3, 5, 14, 0x34da);
                        m_tft->fillRect(441,  6, 5, 11, TFT_BLACK);
                        m_tft->fillRect(434,  9, 5,  8, 0x34da);
                        //m_tft->fillRect(427, 12, 5,  5, 0x34da);
                        break;
                    case 1:
                        //m_tft->fillRect(448,  3, 5, 14, 0x34da);
                        //m_tft->fillRect(441,  6, 5, 11, 0x34da);
                        m_tft->fillRect(434,  9, 5,  8, TFT_BLACK);
                        m_tft->fillRect(427, 12, 5,  5, 0x34da);
                        break;
                    default:
                        m_tft->fillRect(448,  3, 5, 14, 0x34da);
                        //m_tft->fillRect(441,  6, 5, 11, 0x34da);
                        //m_tft->fillRect(434,  9, 5,  8, 0x34da);
                        m_tft->fillRect(427, 12, 5,  5, TFT_BLACK);
                        break;
                    }
                }
                else if (sim.has_new_data_of(STR::SIMData::test_has_new_data_of::RSSI)) {
                    const int rssi = sim.get_rssi();

                    char buf_test[32];
                    snprintf(buf_test, 32, "%i", rssi);
                    const int wd = m_tft->textWidth(buf_test, 2);
                    m_tft->drawString(buf_test, 480 - wd - 2, 2, 2);
                    
                    if (rssi > -65) m_tft->fillRect(448,  3, 5, 14, TFT_BLACK);
                    else            m_tft->fillRect(448,  3, 5, 14, 0x34da);

                    if (rssi > -75) m_tft->fillRect(441,  6, 5, 11, TFT_BLACK);
                    else            m_tft->fillRect(441,  6, 5, 11, 0x34da);

                    if (rssi > -85) m_tft->fillRect(434,  9, 5,  8, TFT_BLACK);
                    else            m_tft->fillRect(434,  9, 5,  8, 0x34da);

                    if (rssi > -95) m_tft->fillRect(427, 12, 5,  5, TFT_BLACK);
                    else            m_tft->fillRect(427, 12, 5,  5, 0x34da);
                }
            }

        }
            break;
        }
        // ================ END OF DRAW BLOCK ================ //
        
        m_screen = next_screen;
    }

    inline Display* DisplayCtl::get_display()
    {
        return m_disp;
    }

    inline void DisplayCtl::set_debugging()
    {
        m_ext_cmd_req = true;
    }

//    inline std::string ensureN(std::string input, size_t last_n) { 
//        return last_n > input.length() ? (input + std::string(last_n - input.length(), ' ')) : input.substr(input.size() - last_n); 
//    }
//
//    inline void Display::Touch::set_down(bool n) {
//        EXCHANGE(w, d, n);
//    }
//
//    inline bool Display::Touch::was_triggered_last_tick() const {
//        return w && !d;
//    }
//    
//    inline Display::Display()
//    {    
//        pinMode(TOUCH_CS, OUTPUT);
//        tft = std::unique_ptr<TFT_eSPI>(new TFT_eSPI());
//
//        tft->init();
//        tft->setRotation(3);
//        tft->fillScreen(TFT_BLACK);
//
//        // load modules 
//        for(uint8_t p = 0; p < static_cast<uint8_t>(CS::device_id::_MAX); ++p)
//        {
//            const CS::device_id conv = static_cast<CS::device_id>(p);
//            m_modules[p].static_name = d2str(conv);
//            m_modules[p].self_id = conv;
//        }
//            
//        
//        tft->fillScreen(TFT_BLACK);
//    }
//
//    inline void Display::Item::draw_resumed(TFT_eSPI* tft, const bool& m_had_transition, bool no_offset) const
//    {
//        const uint16_t real_offy = 20 + (no_offset ? 0 : static_cast<uint16_t>(this->self_id) * 40);
//        const uint16_t color_box = this->was_on ? (this->was_ok ? (this->was_sel ? 0x421d : 0x550a) : 0xb2cb) : 0xa534;
//        const bool upd = m_had_transition || this->state_changed;
//
//        if (upd) {
//            tft->drawFastHLine(0, real_offy, 440, TFT_BLACK);
//            tft->drawFastHLine(0, real_offy + 39, 440, TFT_BLACK);
//
//            //tft->fillRect(0, real_offy, 440, 40, TFT_BLACK);
//            tft->fillRect(0, real_offy + 1, 440, 38, color_box);
//        }
//
//        tft->setTextColor(TFT_WHITE, color_box);
//        tft->setTextSize(1);
//
//        if (upd) {
//            tft->setCursor(8, real_offy + 5, 2);
//            tft->printf("Device: %s ", this->static_name);
//        
//            tft->setCursor(8, real_offy + 20, 2);
//            tft->printf("Connected? %s ", this->was_on ? "yes" : "no");    
//        }
//        
//        if (this->was_on) {
//            if (upd) {
//                tft->setCursor(120, real_offy + 20, 2);
//                tft->printf("Healthy? %s ", this->was_ok ? "yes" : "no");
//            }
//            
//            {
//                const auto _tmp = ensureN(m_ref->get_path(), 19);
//                tft->setCursor(220, real_offy + 5, 2);
//                tft->printf("Last path: %s", _tmp.c_str());
//            }
//            
//            tft->setCursor(220, real_offy + 20, 2);
//            tft->printf("Value: %s  ", m_dat->get_value());
//            //tft->printf("Val: %s", this->last_data_formatted);
//
//            //if (last_data.is_d())      tft->printf("Value: %.2lf  ", last_data.v.d);
//            //else if (last_data.is_f()) tft->printf("Value: %.2f   ", last_data.v.f);
//            //else if (last_data.is_i()) tft->printf("Value: %.2lli ", last_data.v.i);
//            //else if (last_data.is_u()) tft->printf("Value: %.2llu ", last_data.v.u);
//            //else                       tft->printf("Value: <undef>");
//
//        }
//    }
//
//    inline void Display::Item::draw_fullscreen(TFT_eSPI* tft, const bool& m_had_transition) const
//    {
//        const uint16_t color_box = this->was_on ? (this->was_ok ? (this->was_sel ? 0x421d : 0x550a) : 0xb2cb) : 0xa534;
//        const bool upd = m_had_transition || this->state_changed;
//
//        if (upd) {
//            tft->drawFastHLine(0, 0, 440, TFT_BLACK);
//            tft->drawFastHLine(0, 299, 440, TFT_BLACK);
//
//            //tft->fillRect(0, real_offy, 440, 40, TFT_BLACK);
//            tft->fillRect(1, 1, 438, 298, color_box);
//        }
//
//        tft->setTextColor(TFT_WHITE, color_box);
//        tft->setTextSize(1);
//
//        if (upd) {
//            tft->setCursor(8, 5, 2);
//            tft->printf("Device: %s ", this->static_name);
//        
//            tft->setCursor(8, 20, 2);
//            tft->printf("Connected? %s ", this->was_on ? "yes" : "no");    
//        }
//        
//        if (this->was_on) {
//            if (upd) {
//                tft->setCursor(120, 20, 2);
//                tft->printf("Healthy? %s ", this->was_ok ? "yes" : "no");
//            }
//            
//            const ModuleMapping::module_each_map_t& mp = mm.get_data_of(this->self_id);
//            size_t off = 35;
//
//            for(auto it = mp.begin(); it != mp.end(); ++it) 
//            {
//                const auto& last_data = it->second.get_newest();
//                const std::string left_str = "- " + it->first + " => ";
//                
//                if (upd) {
//                    tft->setCursor(10, off, 2);
//                    tft->printf("%s", left_str.c_str());
//                }
//
//                const int16_t off_txt = tft->textWidth(left_str.c_str(), 2);
//                
//                tft->setCursor(10 + off_txt, off, 2);
//
//                if (last_data.is_d())      tft->printf("%.2lf  ", last_data.v.d);
//                else if (last_data.is_f()) tft->printf("%.2f   ", last_data.v.f);
//                else if (last_data.is_i()) tft->printf("%.2lli ", last_data.v.i);
//                else if (last_data.is_u()) tft->printf("%.2llu ", last_data.v.u);
//                else                       tft->printf("<undef>");
//                
//                off += 15;
//            }
//        }
//    }
//
//    inline void Display::Item::self_update()
//    {
//        const bool was_was_on = this->was_on;
//        const bool was_was_ok = this->was_ok;
//
//        this->was_on = m_ref->get_is_online();
//        this->was_ok = !m_ref->get_has_issues();
//
//        this->state_changed = was_was_ok != was_ok || was_was_on != was_on;
//
//        const auto _size = m_ref->size();
//
//        if (!this->was_on) return;
//        if (m_ref->size() == 0) return;
//
//        const auto sec = CPU::get_time_ms() / 1000;    
//        const auto off = sec % _size;
//        
//        this->m_sel = std::next(m_ref->begin(), off)->get();
//        this->m_dat = this->m_sel->get_in_time(0);
//    }
//
//    inline bool Display::Item::self_check_click(Touch& m_touch)
//    {
//        if (m_touch.was_triggered_last_tick()) {
//            const uint16_t real_offy = static_cast<uint16_t>(this->self_id) * 40;
//
//            this->was_sel = (m_touch.y >= real_offy && m_touch.y < (real_offy + 40)) &&
//                            (m_touch.x >= 0 && m_touch.x < 440);
//
//            return this->was_sel;
//        }
//        this->was_sel = false; // reset next tick
//        return false;
//    }
//
//    // has 20 pixels to play with
//    inline void Display::print_top_part(const bool& m_had_transition)
//    {
//        if (m_had_transition) {
//            tft->fillRect(0, 0, 440, 19, TFT_DARKCYAN);
//        }
//        
//        tft->setCursor(1, 1, 2);
//        tft->setTextColor(TFT_WHITE,TFT_BLACK);
//        tft->setTextSize(1);
//
//        tft->printf("Hello world %i", (int)(esp_random() % 1000));
//    }
//
//    // has 20 bottom pixels to play with
//    inline void Display::print_bottom_part()
//    {
//        //if (m_had_transition) {
//        //    tft->fillRect(0, 300, 440, 19, TFT_DARKCYAN);
//        //}
//
//        tft->setCursor(1, 301, 2);
//        tft->setTextColor(TFT_WHITE,TFT_BLACK);
//        tft->setTextSize(1);
//
//        m_last_display_time_to_draw_ms_stabilized = (m_last_display_time_to_draw_ms_stabilized * 9 + m_last_display_time_to_draw_ms) / 10;
//
//        if (CPU::get_time_ms() % 10000 < 5000)
//        {
//            if (!m_time_change_helper || m_had_transition) {
//                tft->fillRect(0, 300, 440, 19, TFT_DARKCYAN);
//                m_time_change_helper = true;
//            }
//
//            tft->printf("M %03hu:%03hu:%c %03i ms S%i CPU %04.1f%%|%04.1f%% %uMHz %s    ", 
//                m_touch.x,
//                m_touch.y,
//                m_touch.d ? (m_touch.w ? 'H' : 'P') : (m_touch.w ? 'v' : '_'),
//                (int)m_last_display_time_to_draw_ms_stabilized,
//                (int)m_state,
//                CPU::get_cpu_usage(0) * 100.0f,
//                CPU::get_cpu_usage(1) * 100.0f,
//                CPU::get_cpu_clock_mhz(),
//                SDcard::sd_get_type()
//            );
//        }
//        else {
//            if (m_time_change_helper || m_had_transition) {
//                tft->fillRect(0, 300, 440, 19, TFT_DARKCYAN);
//                m_time_change_helper = false;
//            }
//            tft->printf("M %03hu:%03hu:%c %03i ms S%i RAM %04.1f%% SD %04.1f%%    ", 
//                m_touch.x,
//                m_touch.y,
//                m_touch.d ? (m_touch.w ? 'H' : 'P') : (m_touch.w ? 'v' : '_'),
//                (int)m_last_display_time_to_draw_ms_stabilized,
//                (int)m_state,
//                CPU::get_ram_usage() * 100.0f,
//                SDcard::sd_used_perc() * 100.0f
//            );
//        }
//    }
//
//    inline void Display::task()
//    {
//        const auto d_b4 = CPU::get_time_ms();
//        const bool m_transition_cleanup_next = m_had_transition;
//
//        using namespace CS;
//
//        ModuleMapping& mm = get_global_map();
//
//        /* TOUCH AREA */
//        m_touch.set_down(tft->getTouch(&m_touch.x, &m_touch.y));
//
//        if (m_had_transition) {
//            tft->fillScreen(TFT_BLACK);
//        }
//
//
//        switch(m_state) {
//        case display_state::MAIN_ALL_MODULES:
//        {
//            print_top_part(m_had_transition);
//            
//            for(size_t p = 0; p < static_cast<size_t>(CS::device_id::_MAX); ++p) {
//                auto& i = m_modules[p];
//                i.self_update(mm);
//                if (i.self_check_click(m_touch)) {
//                    m_selected_module = m_modules + p;
//                    m_state = display_state::SPECIFIC_DEVICE_PLOT;
//                    m_had_transition = true;
//                }
//                i.draw_resumed(tft.get(), m_had_transition);
//            }
//        }
//        break;
//        case display_state::SPECIFIC_DEVICE_PLOT:
//        {
//            m_selected_module->self_update(mm);
//            m_selected_module->draw_fullscreen(tft.get(), m_had_transition, mm);
//
//            if (m_touch.was_triggered_last_tick()) {
//                m_state = display_state::MAIN_ALL_MODULES;
//                m_had_transition = true;
//            }
//        }
//        break;
//        }
//
//        print_bottom_part();
//
//        if (m_transition_cleanup_next) m_had_transition = false;
//        m_last_display_time_to_draw_ms = CPU::get_time_ms() - d_b4;
//    }
}