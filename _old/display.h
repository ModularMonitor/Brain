#pragma once

#include "FS.h"
#include "SPI.h"
#include "TFT/TFT_eSPI.h"
#include "free_fonts.h"
#include "Serial/protocol.h"
#include "Serial/packaging.h"

#include <memory>

#include "data_shared.h"

namespace DP {

    static const char TAG[] = "Display";

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

    // ;-;
    const char* get_fancy_name_for(CS::device_id);

    // Display
    // - Hook stuff to display
    // - Autoload
    class Display {
        std::shared_ptr<TFT_eSPI> m_tft;
        //bool m_is_log_screen = true;        
    public:
        Display();

        TFT_eSPI* operator->();
        const TFT_eSPI* operator->() const;

        //bool is_debugging() const; // check on DisplayCtl to see if thread should call terminal_print() for debugging or not!
        //void toggle_debugging(); // hook to button!
        //void set_debugging(const bool); // forced

        void terminal_print();

        std::shared_ptr<TFT_eSPI> share_tft() const;
    };

    // TouchCtl
    // - Hook stuff to events on touch release
    class TouchCtl {
        struct {
            uint16_t x{}, y{};
            bool state = false;
            uint64_t last_switch_false = 0;
        } m_now, m_b4;
        
        Display& m_disp;
    public:
        TouchCtl(Display&);
        
        // returns true if had touch event (RISING finger)
        bool task();

        // param: old? def false
        uint16_t get_x(bool = false) const;
        // param: old? def false
        uint16_t get_y(bool = false) const;
        // param: old? def false
        uint64_t get_time_ms(bool = false) const;

        uint64_t last_event_was_ms() const;

        int32_t get_dx() const;
        int32_t get_dy() const;
        uint64_t get_delta_time_of_last_ms() const;

        bool is_down() const;

        bool is_touch_on(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h) const;
    };

    class LateralBtnCtl {
    public:
        enum class event_type {
            NO_EVENT, // when nothing was clicked on lateral button ctl
            GO_HOME, // Go back home button
            VERTICAL_ALIGNMENT_CHANGED, // arrow up and down
            GO_DEBUG, // config button
            GO_CONFIG // config button
        };
    private:
        int m_vertical_track = 0; // used in lists. up == --; down == ++
    public:
        event_type touch_event_test(TouchCtl&, const int, const int);
        int get_vertical_pos() const;
        void set_vertical_pos(int);
    };

    void draw_block_at(const char* title, const char* subtitle, const char* subsubtitle, TFT_eSPI* m_tft, const size_t offset, const bool update_body, const bool update, const uint16_t border, const uint16_t fill, const uint16_t font_color = TFT_BLACK);
    void draw_resumed_at(const ::STR::StoredDataEachDevice&, TFT_eSPI*, size_t, const bool&, const bool&, const char*);

    // DisplayCtl:
    // - Controls screen overlays and stuff
    class DisplayCtl {
        enum class screen {
            DEBUG_CMD,      // Shows terminal of logs
            HOME            // Show devices connected, taskbar, buttons for config and so on.
        };

        Display* m_disp = nullptr;
        std::shared_ptr<TFT_eSPI> m_tft; // of m_disp!
        TouchCtl* m_touch = nullptr;

        LateralBtnCtl m_btns;

        screen m_screen = screen::DEBUG_CMD;
        bool m_ext_cmd_req = false;

        CPU::AutoWait m_clock_update_time{1000}; // every second makes sense, used on clock and 1 sec stuff
        CPU::AutoWait m_sdcard_update_time{5000};
        CPU::AutoWait m_idb_update_time{5000};

        // param: with background on last?
        void draw_mouse(bool = true);
        void draw_always_on_top_auto(const bool&, const bool&);
    public:
        void task();

        Display* get_display();

        void set_debugging();
    };

    RUN_ASYNC_ON_CORE_AUTO(DisplayCtl, DisplayTask, task, cpu_core_id_for_display, 2);
}

#include "display.ipp"