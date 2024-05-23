#pragma once
#include "display.h"

#include "common.h"
#include "cpu_ctl.h"
#include "sdcard.h"

namespace DP {

    inline std::string ensureN(std::string input, size_t last_n) { 
        return last_n > input.length() ? (input + std::string(last_n - input.length(), ' ')) : input.substr(input.size() - last_n); 
    }

    inline void Display::Touch::set_down(bool n) {
        EXCHANGE(w, d, n);
    }

    inline bool Display::Touch::was_triggered_last_tick() const {
        return w && !d;
    }
    
    inline Display::Display()
    {    
        pinMode(TOUCH_CS, OUTPUT);
        tft = std::unique_ptr<TFT_eSPI>(new TFT_eSPI());

        tft->init();
        tft->setRotation(3);
        tft->fillScreen(TFT_BLACK);

        // load modules 
        for(uint8_t p = 0; p < static_cast<uint8_t>(device_id::_MAX); ++p)
        {
            const device_id conv = static_cast<device_id>(p);
            m_modules[p].static_name = d2str(conv);
            m_modules[p].self_id = conv;
        }
        
        /*{
            bool calibrate_loaded = false;
            if (!SPIFFS.begin()) {
                SPIFFS.format();
                SPIFFS.begin();
            }

            if (SPIFFS.exists(CALIBRATION_FILE)) {
                auto f = SPIFFS.open(CALIBRATION_FILE, "r");
                if (f) {
                    calibrate_loaded |= (f.readBytes((char *)calibrationData, 14) == 14);
                    f.close();
                }
            }
            if (calibrate_loaded) {
                tft->setTouch(calibrationData);
            }
            else {
                tft->calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
                
                auto f = SPIFFS.open(CALIBRATION_FILE, "w");
                if (f) {
                    f.write((const unsigned char *)calibrationData, 14);
                    f.close();
                }
            }
        }*/
            
        
        tft->fillScreen(TFT_BLACK);
    }

    inline void Display::Item::draw_resumed(TFT_eSPI* tft, const bool& m_had_transition, bool no_offset) const
    {
        const uint16_t real_offy = 20 + (no_offset ? 0 : static_cast<uint16_t>(this->self_id) * 40);
        const uint16_t color_box = this->was_on ? (this->was_ok ? (this->was_sel ? 0x421d : 0x550a) : 0xb2cb) : 0xa534;
        const bool upd = m_had_transition || this->state_changed;

        if (upd) {
            tft->drawFastHLine(0, real_offy, 440, TFT_BLACK);
            tft->drawFastHLine(0, real_offy + 39, 440, TFT_BLACK);

            //tft->fillRect(0, real_offy, 440, 40, TFT_BLACK);
            tft->fillRect(0, real_offy + 1, 440, 38, color_box);
        }

        tft->setTextColor(TFT_WHITE, color_box);
        tft->setTextSize(1);

        if (upd) {
            tft->setCursor(8, real_offy + 5, 2);
            tft->printf("Device: %s ", this->static_name);
        
            tft->setCursor(8, real_offy + 20, 2);
            tft->printf("Connected? %s ", this->was_on ? "yes" : "no");    
        }
        
        if (this->was_on) {
            if (upd) {
                tft->setCursor(120, real_offy + 20, 2);
                tft->printf("Healthy? %s ", this->was_ok ? "yes" : "no");
            }
            
            {
                const auto _tmp = ensureN(this->last_data_str, 19);
                tft->setCursor(220, real_offy + 5, 2);
                tft->printf("Last path: %s", _tmp.c_str());
            }
            
            tft->setCursor(220, real_offy + 20, 2);
            //tft->printf("Val: %s", this->last_data_formatted);

            if (last_data.is_d())      tft->printf("Value: %.2lf  ", last_data.v.d);
            else if (last_data.is_f()) tft->printf("Value: %.2f   ", last_data.v.f);
            else if (last_data.is_i()) tft->printf("Value: %.2lli ", last_data.v.i);
            else if (last_data.is_u()) tft->printf("Value: %.2llu ", last_data.v.u);
            else                       tft->printf("Value: <undef>");

        }
    }

    inline void Display::Item::draw_fullscreen(TFT_eSPI* tft, const bool& m_had_transition, ModuleMapping& mm) const
    {
        const uint16_t color_box = this->was_on ? (this->was_ok ? (this->was_sel ? 0x421d : 0x550a) : 0xb2cb) : 0xa534;
        const bool upd = m_had_transition || this->state_changed;

        if (upd) {
            tft->drawFastHLine(0, 0, 440, TFT_BLACK);
            tft->drawFastHLine(0, 299, 440, TFT_BLACK);

            //tft->fillRect(0, real_offy, 440, 40, TFT_BLACK);
            tft->fillRect(1, 1, 438, 298, color_box);
        }

        tft->setTextColor(TFT_WHITE, color_box);
        tft->setTextSize(1);

        if (upd) {
            tft->setCursor(8, 5, 2);
            tft->printf("Device: %s ", this->static_name);
        
            tft->setCursor(8, 20, 2);
            tft->printf("Connected? %s ", this->was_on ? "yes" : "no");    
        }
        
        if (this->was_on) {
            if (upd) {
                tft->setCursor(120, 20, 2);
                tft->printf("Healthy? %s ", this->was_ok ? "yes" : "no");
            }
            
            const ModuleMapping::module_each_map_t& mp = mm.get_data_of(this->self_id);
            size_t off = 35;

            for(auto it = mp.begin(); it != mp.end(); ++it) 
            {
                const auto& last_data = it->second.get_newest();
                const std::string left_str = "- " + it->first + " => ";
                
                if (upd) {
                    tft->setCursor(10, off, 2);
                    tft->printf("%s", left_str.c_str());
                }

                const int16_t off_txt = tft->textWidth(left_str.c_str(), 2);
                
                tft->setCursor(10 + off_txt, off, 2);

                if (last_data.is_d())      tft->printf("%.2lf  ", last_data.v.d);
                else if (last_data.is_f()) tft->printf("%.2f   ", last_data.v.f);
                else if (last_data.is_i()) tft->printf("%.2lli ", last_data.v.i);
                else if (last_data.is_u()) tft->printf("%.2llu ", last_data.v.u);
                else                       tft->printf("<undef>");
                
                off += 15;
            }
        }
    }

    inline void Display::Item::self_update(ModuleMapping& mm)
    {
        const bool was_was_on = this->was_on;
        const bool was_was_ok = this->was_ok;

        this->was_on = mm.get_online(this->self_id);
        this->was_ok = !mm.get_has_issues(this->self_id);

        this->state_changed = was_was_ok != was_ok || was_was_on != was_on;

        if (!this->was_on) return;

        const ModuleMapping::module_each_map_t& mp = mm.get_data_of(this->self_id);

        if (mp.size() == 0) return;

        const auto sec = millis() / 1000;    
        const auto off = sec % mp.size();
        const auto& iter = std::next(mp.begin(), off);
        const ModuleMapping::module_data_circular& sel = iter->second;

        this->last_data_str = iter->first;
        this->last_data = sel.get_newest();
    }

    inline bool Display::Item::self_check_click(Touch& m_touch)
    {
        if (m_touch.was_triggered_last_tick()) {
            const uint16_t real_offy = static_cast<uint16_t>(this->self_id) * 40;

            this->was_sel = (m_touch.y >= real_offy && m_touch.y < (real_offy + 40)) &&
                            (m_touch.x >= 0 && m_touch.x < 440);

            return this->was_sel;
        }
        this->was_sel = false; // reset next tick
        return false;
    }

    // has 20 pixels to play with
    inline void Display::print_top_part(const bool& m_had_transition)
    {
        if (m_had_transition) {
            tft->fillRect(0, 0, 440, 19, TFT_DARKCYAN);
        }
        
        tft->setCursor(1, 1, 2);
        tft->setTextColor(TFT_WHITE,TFT_BLACK);
        tft->setTextSize(1);

        tft->printf("Hello world %i", (int)(esp_random() % 1000));
    }

    // has 20 bottom pixels to play with
    inline void Display::print_bottom_part()
    {
        //if (m_had_transition) {
        //    tft->fillRect(0, 300, 440, 19, TFT_DARKCYAN);
        //}

        tft->setCursor(1, 301, 2);
        tft->setTextColor(TFT_WHITE,TFT_BLACK);
        tft->setTextSize(1);

        m_last_display_time_to_draw_ms_stabilized = (m_last_display_time_to_draw_ms_stabilized * 9 + m_last_display_time_to_draw_ms) / 10;

        if (CPU::get_time_ms() % 10000 < 5000)
        {
            if (!m_time_change_helper || m_had_transition) {
                tft->fillRect(0, 300, 440, 19, TFT_DARKCYAN);
                m_time_change_helper = true;
            }

            tft->printf("M %03hu:%03hu:%c %03i ms S%i CPU %04.1f%%|%04.1f%% %uMHz %s    ", 
                m_touch.x,
                m_touch.y,
                m_touch.d ? (m_touch.w ? 'H' : 'P') : (m_touch.w ? 'v' : '_'),
                (int)m_last_display_time_to_draw_ms_stabilized,
                (int)m_state,
                CPU::get_cpu_usage(0) * 100.0f,
                CPU::get_cpu_usage(1) * 100.0f,
                CPU::get_cpu_clock_mhz(),
                SDcard::sd_get_type()
            );
        }
        else {
            if (m_time_change_helper || m_had_transition) {
                tft->fillRect(0, 300, 440, 19, TFT_DARKCYAN);
                m_time_change_helper = false;
            }
            tft->printf("M %03hu:%03hu:%c %03i ms S%i RAM %04.1f%% SD %04.1f%%    ", 
                m_touch.x,
                m_touch.y,
                m_touch.d ? (m_touch.w ? 'H' : 'P') : (m_touch.w ? 'v' : '_'),
                (int)m_last_display_time_to_draw_ms_stabilized,
                (int)m_state,
                CPU::get_ram_usage() * 100.0f,
                SDcard::sd_used_perc() * 100.0f
            );
        }
    }

    inline void Display::task()
    {
        const auto d_b4 = millis();
        const bool m_transition_cleanup_next = m_had_transition;

        using namespace CS;

        ModuleMapping& mm = get_global_map();

        /* TOUCH AREA */
        m_touch.set_down(tft->getTouch(&m_touch.x, &m_touch.y));

        if (m_had_transition) {
            tft->fillScreen(TFT_BLACK);
        }


        switch(m_state) {
        case display_state::MAIN_ALL_MODULES:
        {
            print_top_part(m_had_transition);
            
            for(size_t p = 0; p < static_cast<size_t>(device_id::_MAX); ++p) {
                auto& i = m_modules[p];
                i.self_update(mm);
                if (i.self_check_click(m_touch)) {
                    m_selected_module = m_modules + p;
                    m_state = display_state::SPECIFIC_DEVICE_PLOT;
                    m_had_transition = true;
                }
                i.draw_resumed(tft.get(), m_had_transition);
            }
        }
        break;
        case display_state::SPECIFIC_DEVICE_PLOT:
        {
            m_selected_module->self_update(mm);
            m_selected_module->draw_fullscreen(tft.get(), m_had_transition, mm);

            if (m_touch.was_triggered_last_tick()) {
                m_state = display_state::MAIN_ALL_MODULES;
                m_had_transition = true;
            }
        }
        break;
        }

        print_bottom_part();

        if (m_transition_cleanup_next) m_had_transition = false;
        m_last_display_time_to_draw_ms = millis() - d_b4;
    }

    /*inline void loop_display(void* arg_useless)
    {
        Display disp;

        while(1) {
            delayMicroseconds(1);
            disp.task();
        }
    }*/
}