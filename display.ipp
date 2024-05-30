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
        // 40 x 46 icon home
        PROGMEM const unsigned char config_icon_home[] = {
            0x00, 0x01, 0xF8, 0x00, 0x00, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x00, 0x1F, 
            0xFF, 0x80, 0x00, 0x00, 0x7F, 0xFF, 0xE0, 0x00, 0x01, 0xFF, 0xFF, 0xF8, 
            0x00, 0x07, 0xFF, 0xFF, 0xFE, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0x80, 0x7F, 
            0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 
            0xFF, 0xF0, 0xFC, 0x00, 0x00, 0x03, 0xF0, 0x70, 0x00, 0x00, 0x00, 0xE0, 
            0x0E, 0x00, 0x00, 0x07, 0x00, 0x1F, 0x00, 0x00, 0x0F, 0x80, 0x1F, 0x00, 
            0x00, 0x0F, 0x80, 0x1F, 0x00, 0x00, 0x0F, 0x80, 0x1F, 0x00, 0x00, 0x0F, 
            0x80, 0x1F, 0x00, 0x00, 0x0F, 0x80, 0x1F, 0x00, 0x00, 0x0F, 0x80, 0x1F, 
            0x00, 0xFF, 0xCF, 0x80, 0x1F, 0x00, 0xFF, 0xCF, 0x80, 0x1F, 0x00, 0xFF, 
            0xCF, 0x80, 0x1F, 0x00, 0xFE, 0x0F, 0x80, 0x1F, 0x00, 0xFF, 0x0F, 0x80, 
            0x1F, 0x00, 0xFF, 0x8F, 0x80, 0x1F, 0x00, 0xEF, 0xCF, 0x80, 0x1F, 0x00, 
            0xE3, 0xCF, 0x80, 0x1F, 0x00, 0xE1, 0xEF, 0x80, 0x1F, 0x00, 0xE1, 0xF7, 
            0x80, 0x1F, 0x00, 0x00, 0xF7, 0x80, 0x1F, 0x00, 0x00, 0xFB, 0x80, 0x1F, 
            0xFF, 0xFF, 0x7B, 0x80, 0x1F, 0xFF, 0xFF, 0x7D, 0x80, 0x1F, 0xFF, 0xFF, 
            0xBD, 0x80, 0x1F, 0xFF, 0xFF, 0xBD, 0x80, 0x0F, 0xFF, 0xFF, 0xBD, 0x00, 
            0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 
            0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x03, 0xF0, 
            0x00, 0x00, 0x70, 0x07, 0xE0, 0x00, 0x00, 0x7C, 0x3F, 0xC0, 0x00, 0x00, 
            0x7F, 0xFF, 0x80, 0x00, 0x00, 0x3F, 0xFE, 0x00, 0x00, 0x00, 0x07, 0xF8, 
            0x00, 0x00
        };
        // 26 x 35
        PROGMEM const unsigned char config_icon_up[] = {
            0x00, 0x7F, 0x80, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x01, 0xFF, 0xE0, 0x00, 
            0x01, 0xFF, 0xE0, 0x00, 0x03, 0xFF, 0xF0, 0x00, 0x07, 0xFF, 0xF8, 0x00, 
            0x07, 0xFF, 0xF8, 0x00, 0x0F, 0xFF, 0xFC, 0x00, 0x0F, 0xBF, 0x7C, 0x00, 
            0x1F, 0xBF, 0x7E, 0x00, 0x1F, 0x3F, 0x3E, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 
            0x7E, 0x3F, 0x1F, 0x80, 0x7E, 0x3F, 0x1F, 0x80, 0xFC, 0x3F, 0x0F, 0xC0, 
            0xF8, 0x3F, 0x07, 0xC0, 0xF8, 0x3F, 0x07, 0xC0, 0x70, 0x3F, 0x03, 0x80, 
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00
        };
        // 26 x 35
        PROGMEM const unsigned char config_icon_down[] = {
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
            0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x70, 0x3F, 0x03, 0x80, 
            0xF8, 0x3F, 0x07, 0xC0, 0xF8, 0x3F, 0x07, 0xC0, 0xFC, 0x3F, 0x0F, 0xC0, 
            0x7E, 0x3F, 0x1F, 0x80, 0x7E, 0x3F, 0x1F, 0x80, 0x3F, 0x3F, 0x3F, 0x00, 
            0x1F, 0x3F, 0x3E, 0x00, 0x1F, 0xBF, 0x7E, 0x00, 0x0F, 0xBF, 0x7C, 0x00, 
            0x0F, 0xFF, 0xFC, 0x00, 0x07, 0xFF, 0xF8, 0x00, 0x07, 0xFF, 0xF8, 0x00, 
            0x03, 0xFF, 0xF0, 0x00, 0x01, 0xFF, 0xE0, 0x00, 0x01, 0xFF, 0xE0, 0x00, 
            0x00, 0xFF, 0xC0, 0x00, 0x00, 0x7F, 0x80, 0x00
        };
        // 32 x 38
        PROGMEM const unsigned char config_icon_debug[] = {
            0x1F, 0xFF, 0xFF, 0xF8, 0x7F, 0xFF, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0xFE, 
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xFF, 0xFF, 0xFF, 
            0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF, 
            0xFE, 0x07, 0xFF, 0xFF, 0xFF, 0x03, 0xFF, 0xFF, 0xFF, 0xC3, 0xFF, 0xFF, 
            0xFF, 0xC3, 0xFF, 0xFF, 0xFF, 0x03, 0xFF, 0xFF, 0xFE, 0x07, 0xFF, 0xFF, 
            0xF8, 0x1F, 0xFF, 0xFF, 0xF0, 0x3F, 0xC0, 0x0F, 0xF0, 0xFF, 0xC0, 0x0F, 
            0xF1, 0xFF, 0xC0, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFE, 
            0x7F, 0xFF, 0xFF, 0xFE, 0x1F, 0xFF, 0xFF, 0xF8
        };
        // 32 x 32
        PROGMEM const unsigned char config_icon_config[] = {
            0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 
            0x03, 0x07, 0xE0, 0xC0, 0x07, 0x87, 0xE1, 0xE0, 0x0F, 0xCF, 0xF3, 0xF0, 
            0x1F, 0xFF, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF, 0xF8, 0x0F, 0xFF, 0xFF, 0xF0, 
            0x07, 0xFF, 0xFF, 0xE0, 0x03, 0xFC, 0x3F, 0xC0, 0x03, 0xF0, 0x0F, 0xC0, 
            0x07, 0xE0, 0x07, 0xE0, 0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0xC0, 0x03, 0xFF, 
            0xFF, 0xC0, 0x03, 0xFF, 0xFF, 0xC0, 0x03, 0xFF, 0xFF, 0xC0, 0x03, 0xFF, 
            0xFF, 0xE0, 0x07, 0xFF, 0x07, 0xE0, 0x07, 0xE0, 0x03, 0xF0, 0x0F, 0xC0, 
            0x03, 0xFC, 0x3F, 0xC0, 0x07, 0xFF, 0xFF, 0xE0, 0x0F, 0xFF, 0xFF, 0xF0, 
            0x1F, 0xFF, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF, 0xF8, 0x0F, 0xCF, 0xF3, 0xF0, 
            0x07, 0x87, 0xE1, 0xE0, 0x03, 0x07, 0xE0, 0xC0, 0x00, 0x07, 0xE0, 0x00, 
            0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00
        };
        // 24 x 24 icon close
        PROGMEM const unsigned char terminal_icon_close[] = {
            0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFE, 0x63, 0xFF, 0xC6, 
            0x61, 0xFF, 0x86, 0x60, 0xFF, 0x06, 0x70, 0x7E, 0x0E, 0x78, 0x3C, 0x1E, 
            0x7C, 0x18, 0x3E, 0x7E, 0x00, 0x7E, 0x7F, 0x00, 0xFE, 0x7F, 0x81, 0xFE, 
            0x7F, 0x81, 0xFE, 0x7F, 0x00, 0xFE, 0x7E, 0x00, 0x7E, 0x7C, 0x18, 0x3E, 
            0x78, 0x3C, 0x1E, 0x70, 0x7E, 0x0E, 0x60, 0xFF, 0x06, 0x61, 0xFF, 0x86, 
            0x63, 0xFF, 0xC6, 0x7F, 0xFF, 0xFE, 0x3F, 0xFF, 0xFC, 0x00, 0x00, 0x00,
            0                 
        };

    }
    
    inline const char* get_fancy_name_for(CS::device_id id)
    {
        switch(id) {
        case CS::device_id::DHT22_SENSOR:       return "Modulo de temperatura e humidade";
        case CS::device_id::MICS_6814_SENSOR:   return "Modulo de NO2, NH3 e CO";
        case CS::device_id::LY038_HW072_SENSOR: return "Modulo de som e luz";
        case CS::device_id::GY87_SENSOR:        return "Modulo acelerometro, giroscopio, barometro";
        case CS::device_id::CCS811_SENSOR:      return "Modulo de eCO2 e TVOC";
        case CS::device_id::PMSDS011_SENSOR:    return "Modulo de particulas no ar";
        case CS::device_id::BATTERY_SENSOR:     return "Modulo da bateria";
        default:                                return "Desconhecido";
        }
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



    LateralBtnCtl::event_type LateralBtnCtl::touch_event_test(TouchCtl& t, const int min_val, const int max_val)
    {
        uint16_t buttons{};
        for(size_t p = 0; p < 5; ++p) buttons |= (static_cast<uint16_t>(t.is_touch_on(480 - bar_right_width, bar_top_height + (p * bar_right_each_height), 40, 60)) << p);

        switch(buttons) {
        case 0x01: // home
            return event_type::GO_HOME;
        case 0x02: // up
            if (m_vertical_track < max_val) ++m_vertical_track;
            return event_type::VERTICAL_ALIGNMENT_CHANGED;
        case 0x04: // down
            if (m_vertical_track > min_val) --m_vertical_track;
            return event_type::VERTICAL_ALIGNMENT_CHANGED;
            break;
        case 0x08: // debug
            return event_type::GO_DEBUG;
        case 0x10: // config
            return event_type::GO_CONFIG;
        default: // if somehow
            return event_type::NO_EVENT;
        }
    }

    int LateralBtnCtl::get_vertical_pos() const
    {
        return m_vertical_track;
    }

    void LateralBtnCtl::set_vertical_pos(int v)
    {
        m_vertical_track = v;
    }



    inline void draw_block_at(const char* title, const char* subtitle, const char* subsubtitle, TFT_eSPI* m_tft,
        const size_t offset, const bool update, const bool update_body, const uint16_t border,
        const uint16_t fill, const uint16_t font_color)
    {
        const int32_t base_y = static_cast<int32_t>(21 + (offset * item_resumed_height_max));

        if (base_y > 320) return;

        if (update_body) {
            m_tft->fillRoundRect(
                1, base_y,
                item_resumed_width_max - 2, item_resumed_height_max - 2, item_resumed_border_radius,
                border);
            m_tft->fillRoundRect(
                3, base_y + 2,
                item_resumed_width_max - 6, item_resumed_height_max - 6, item_resumed_border_radius,
                fill);
        }

        if (update || update_body) {
            m_tft->setTextColor(font_color, fill);

            if (subsubtitle) {
                m_tft->drawString(title, 6, base_y + 5, 2);
                m_tft->drawString(subtitle, 6, base_y + 20, 2);
                m_tft->drawString(subsubtitle, 6, base_y + 35, 2);
            }
            else {
                m_tft->drawString(title, 6, base_y + 5, 2);
                m_tft->drawString(subtitle, 6, base_y + 30, 2);
            }

        }
    }

    inline void draw_resumed_at(::STR::StoredDataEachDevice& m_dev, TFT_eSPI* m_tft, const size_t item_in_list, const bool& state_changed, const char* name)
    {
        const uint16_t bgcolor = m_dev.get_has_issues() ? item_has_issues_bg_color : (m_dev.get_is_online() ? item_online_bg_color : TFT_DARKGREY);
        const uint16_t bordercolor = m_dev.get_has_issues() ? item_has_issues_bg_color_border : (m_dev.get_is_online() ? item_online_bg_color_border : TFT_DARKGREY);

        const size_t siz = m_dev.size();
        const char no_data[] = "sem dados";

        if (!siz) {
            draw_block_at(name, no_data, nullptr, m_tft, item_in_list,
                state_changed, state_changed || m_dev.has_new_data_for_display(), bordercolor,
                bgcolor, TFT_BLACK);
            return;
        }

        const size_t                                    off_by_time = (CPU::get_time_ms() / 2000) % siz;
        const auto                                      item        = m_dev[off_by_time];
        const ::STR::RepresentedData::data_storage*     data        = item ? item->get_in_time(0) : nullptr;
        
        const char* path = item ? item->get_path() : no_data;
        const char* value = data ? data->get_value() : no_data;
        const uint64_t time_since_update_sec = data ? ((CPU::get_time_ms() - data->get_modified_ms()) / 1000) : 0;

        char minibuf[128];
        char secnbuf[64];
        snprintf(minibuf, 128, "%s = %s  ", path, value);
        snprintf(secnbuf, 64, "(%" PRIu64 " seg atras)  ", time_since_update_sec);

        draw_block_at(name, minibuf, item ? secnbuf : nullptr, m_tft, item_in_list, state_changed, m_dev.has_new_data_for_display(), bordercolor, bgcolor, TFT_BLACK);
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

    inline void DisplayCtl::draw_always_on_top_auto(const bool& state_changed)
    {
        STR::SharedData& shared_data = STR::get_singleton_of_SharedData();

        STR::SIMData& sim = shared_data.get_sim_data();
        STR::InfluxDBData& idb = shared_data.get_idb_data();

        m_tft->setTextSize(1);

        if (state_changed) {
            m_tft->fillRect(0, 0, 480, bar_top_height, bar_top_color); // top bar bg
            m_tft->fillRect(480 - bar_right_width, bar_top_height, 480, 320, bar_right_color); // right bar menu
            m_tft->fillRect(0, 20, 440, 320, TFT_WHITE); // body

            m_tft->setTextColor(TFT_BLACK, bar_top_color); // top bar

            m_tft->drawString("Waiting 4G...", 2, 2, 2);
            m_tft->drawString("...", 457, 2, 2);
            m_tft->drawString("4G", 406, 2, 2);

            m_tft->drawBitmap(376, 4, Bitmaps::sd_card_icon_11_13, 11, 13, TFT_BLACK);
            m_tft->drawBitmap(289, 3, Bitmaps::database_icon_11_14, 11, 14, TFT_BLACK);

            m_tft->drawBitmap(442,  28, Bitmaps::config_icon_home,   40, 46, TFT_BLACK);
            m_tft->drawBitmap(447,  92, Bitmaps::config_icon_up,     26, 35, TFT_BLACK);
            m_tft->drawBitmap(447, 152, Bitmaps::config_icon_down,   26, 35, TFT_BLACK);
            m_tft->drawBitmap(444, 211, Bitmaps::config_icon_debug,  32, 38, TFT_BLACK);
            m_tft->drawBitmap(444, 274, Bitmaps::config_icon_config, 32, 32, TFT_BLACK);
        }
        else {
            m_tft->setTextColor(TFT_BLACK, bar_top_color); // top bar

            // = = = = = = TIME (clock) = = = = = = //
            if (sim.has_new_data_for_display(STR::SIMData::test_has_new_data_of::TIME) || (m_clock_update_time.is_time() && sim.get_rssi() != -1)) {
                char safe_buffer[96];
                m_tft->drawString(sim.get_time(safe_buffer, 96), 2, 2, 2);
            }

            // = = = = = = SD Card status = = = = = = //
            if (m_sdcard_update_time.is_time()) {
                const char* stat = SDcard::sd_get_type();
                const int wd = m_tft->textWidth(stat, 2);

                m_tft->fillRect(304, 2, 70, 16, bar_top_color); // fill color
                m_tft->drawString(stat, 372 - wd, 2, 2);
            }

            // = = = = = = DB connection = = = = = = //
            if (m_idb_update_time.is_time()) {
                const char* stat = idb.get_is_online() ? "ONLINE" : "OFFLINE";
                const int wd = m_tft->textWidth(stat, 2);

                m_tft->fillRect(200, 2, 87, 16, bar_top_color); // fill color
                m_tft->drawString(stat, 287 - wd - 2, 2, 2);
            }
            
            // = = = = = = RSSI (bar and text) = = = = = = //
            if (sim.get_rssi() == -1) {
                m_tft->drawString("??", 457, 2, 2);

                switch((CPU::get_time_ms() / 200) % 4){
                case 3:
                    m_tft->fillRect(448,  3, 5, 14, TFT_BLACK);
                    m_tft->fillRect(441,  6, 5, 11, bar_top_color);
                    //m_tft->fillRect(434,  9, 5,  8, bar_top_color);
                    //m_tft->fillRect(427, 12, 5,  5, bar_top_color);
                    break;
                case 2:
                    //m_tft->fillRect(448,  3, 5, 14, bar_top_color);
                    m_tft->fillRect(441,  6, 5, 11, TFT_BLACK);
                    m_tft->fillRect(434,  9, 5,  8, bar_top_color);
                    //m_tft->fillRect(427, 12, 5,  5, bar_top_color);
                    break;
                case 1:
                    //m_tft->fillRect(448,  3, 5, 14, bar_top_color);
                    //m_tft->fillRect(441,  6, 5, 11, bar_top_color);
                    m_tft->fillRect(434,  9, 5,  8, TFT_BLACK);
                    m_tft->fillRect(427, 12, 5,  5, bar_top_color);
                    break;
                default:
                    m_tft->fillRect(448,  3, 5, 14, bar_top_color);
                    //m_tft->fillRect(441,  6, 5, 11, bar_top_color);
                    //m_tft->fillRect(434,  9, 5,  8, bar_top_color);
                    m_tft->fillRect(427, 12, 5,  5, TFT_BLACK);
                    break;
                }
            }
            else if (sim.has_new_data_for_display(STR::SIMData::test_has_new_data_of::RSSI)) {
                const int rssi = sim.get_rssi();

                char buf_test[32];
                snprintf(buf_test, 32, "%i", rssi);
                const int wd = m_tft->textWidth(buf_test, 2);
                m_tft->drawString(buf_test, 480 - wd - 2, 2, 2);
                
                if (rssi > -65) m_tft->fillRect(448,  3, 5, 14, TFT_BLACK);
                else            m_tft->fillRect(448,  3, 5, 14, bar_top_color);

                if (rssi > -75) m_tft->fillRect(441,  6, 5, 11, TFT_BLACK);
                else            m_tft->fillRect(441,  6, 5, 11, bar_top_color);

                if (rssi > -85) m_tft->fillRect(434,  9, 5,  8, TFT_BLACK);
                else            m_tft->fillRect(434,  9, 5,  8, bar_top_color);

                if (rssi > -95) m_tft->fillRect(427, 12, 5,  5, TFT_BLACK);
                else            m_tft->fillRect(427, 12, 5,  5, bar_top_color);
            }
        }

    }

    inline void DisplayCtl::task()
    {
        // track screen change
        screen next_screen = m_screen;
        STR::SharedData& shared_data = STR::get_singleton_of_SharedData();

        // Guarantee everything is loaded
        if (!m_disp) {
            m_disp = new Display();
            m_touch = new TouchCtl(*m_disp);
            m_tft = m_disp->share_tft();

            LOGI(TAG, "Loaded all modules. Ready to start UI...");
            m_disp->terminal_print();

            for(float f = 0.5f; f < 1.0f; f += 0.025f) {
                delay(25);
                SET_DISPLAY_BRIGHTNESS(f);
            }
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
                if (m_touch->is_touch_on(456, 0, 24, 24))
                    next_screen = screen::HOME;
            }
                break;
            default:
            {
                const auto cmd = m_btns.touch_event_test(*m_touch, -2, 0); // = = = = = = = = = = TODO TBD LIMIT HERE OF ITEMS TO SCROLL, this is dynamic and depends on current mode

                switch(cmd) {
                case LateralBtnCtl::event_type::GO_HOME: // Go back home button
                    next_screen = screen::HOME;
                    m_btns.set_vertical_pos(0);
                    break;
                case LateralBtnCtl::event_type::VERTICAL_ALIGNMENT_CHANGED: // arrow up and down
                    break;
                case LateralBtnCtl::event_type::GO_DEBUG: // config button
                    m_btns.set_vertical_pos(0);
                    next_screen = screen::DEBUG_CMD;
                    break;
                case LateralBtnCtl::event_type::GO_CONFIG: // config button
                    m_btns.set_vertical_pos(0);
                    break;
                case LateralBtnCtl::event_type::NO_EVENT: 
                {

                }
                    break;
                }
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

        switch(next_screen) {
        case screen::DEBUG_CMD: // only log screen
        {
            const bool update_forced = (!m_touch->is_down() && m_touch->last_event_was_ms() < 400);

            if (state_changed) m_tft->fillScreen(TFT_DARKGREEN);

            // if had new line on log or touch stuff
            if (LG::get_singleton_of_Logger().for_display_had_news() || update_forced || state_changed) {
                m_disp->terminal_print();
                //m_tft->fillRect(460, 0, 20, 20, TFT_RED);
                //m_tft->drawLine(462, 2, 478, 18, 0xA000);
                //m_tft->drawLine(478, 18, 462, 2, 0xA000);
                m_tft->drawBitmap(456, 0, Bitmaps::terminal_icon_close, 24, 24, item_close_button_x_and_border, item_close_button_body);
            }

            // touch shenanigans
            if (m_touch->is_down() || update_forced) {
                draw_mouse();
            }
        }
            break;
        case screen::HOME:
        {
            draw_always_on_top_auto(state_changed);
            for(int p = 0; p < CS::d2u(CS::device_id::_MAX); ++p) {
                const int real_p = p - m_btns.get_vertical_pos();
                if (real_p < 0 || real_p >= (int)CS::d2u(CS::device_id::_MAX)) continue;
                draw_resumed_at(
                    shared_data(static_cast<CS::device_id>(real_p)),
                    m_tft.get(),
                    p,
                    state_changed || had_touch_event,
                    get_fancy_name_for(static_cast<CS::device_id>(real_p)));
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