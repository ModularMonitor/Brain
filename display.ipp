#pragma once
#include "display.h"

#include "common.h"
#include "cpu_ctl.h"
#include "sdcard.h"

namespace DP {

    inline Display::Display()
        : m_tft(std::unique_ptr<TFT_eSPI>(new TFT_eSPI()))
    {
        m_tft->init();
        m_tft->setRotation(3);
        m_tft->fillScreen(TFT_BLACK);

        //m_tft->setTextColor(TFT_WHITE, TFT_BLACK);
        //m_tft->setTextSize(1);
        //m_tft->setCursor(0, 0, 2);

        LOGI(TAG, "## ModularMonitor loading screen ##");
        LOGI(TAG, "> Loading screen");
        LOGI(TAG, "Checking file " DISPLAY_CALIBRATION_FILE "...");
        this->terminal_print();

        if (!SDcard::is_sd_init()) {
            LOGE(TAG, "SD card is not loaded / present! Touch calibration skipped.");
            this->terminal_print();
        }
        else { // has SD card

            uint16_t calibrationData[6]{}; // 14 bytes actually + 1 extra byte for flag

            CPU::run_on_core_sync([](void* a){
                uint16_t* calData = (uint16_t*)a;

                File f = SDcard::f_open(DISPLAY_CALIBRATION_FILE, "r");

                if (!f || (f.readBytes((char*)calData, 14) != 14)) {
                    LOGI(TAG, DISPLAY_CALIBRATION_FILE " not found. Calibration to go.");
                    calData[5] = 2;
                }
                else {
                    LOGI(TAG, DISPLAY_CALIBRATION_FILE " found and ready to set!");
                    calData[5] = 1;
                }
                f.close();
            }, cpu_core_id_for_sd_card, (void*)calibrationData);

            this->terminal_print();

            switch(calibrationData[5]) {
            case 1:
            {
                LOGI(TAG, "Found display calibration. Applying... ");
                m_tft->setTouch(calibrationData);
                LOGI(TAG, DISPLAY_CALIBRATION_FILE " applied!");
                this->terminal_print();
            }
                break;
            case 2:
            {
                LOGI(TAG, "No file or data found, initializing calibration...");
                LOGI(TAG, "Calibrating display... Please proceed.");
                this->terminal_print();

                delay(1000);
                
                m_tft->fillScreen(TFT_BLACK);
                m_tft->calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
                m_tft->fillScreen(TFT_BLACK);

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

                this->terminal_print();
            }
                break;
            default:
                LOGE(TAG, "Undefined behaviour on Display::Display. Skipping calibration.");
                this->terminal_print();
                break;
            }
        }

        LOGI(TAG, "Ready.");
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

    inline bool Display::is_debugging() const
    {
        return m_is_log_screen;
    }

    inline void Display::toggle_debugging()
    {
        m_is_log_screen = !m_is_log_screen;
        if (m_is_log_screen) { LOGI(TAG, "Debug screen enabled!"); }
        else                 { LOGI(TAG, "Debug screen disabled!"); }
    }

    inline void Display::set_debugging(const bool b)
    {
        m_is_log_screen = b;
        if (m_is_log_screen) { LOGI(TAG, "Debug screen enabled!"); }
        else                 { LOGI(TAG, "Debug screen disabled!"); }
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

    inline void DisplayCtl::task()
    {
        if (!m_disp) m_disp = new Display();

        // make fancy tasking logic later

        if (m_disp->is_debugging()) {
            m_disp->terminal_print();
            return;
        }

        // make fancy draw logic later
    }

    Display* DisplayCtl::get_display()
    {
        return m_disp;
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