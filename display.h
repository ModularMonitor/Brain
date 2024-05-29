#pragma once

#include "FS.h"
#include "SPI.h"
#include "TFT/TFT_eSPI.h"
#include "free_fonts.h"
#include "Serial/protocol.h"
#include "Serial/packaging.h"

#include <memory>

#include "data_displayed.h"

namespace DP {

    static const char TAG[] = "Display";

    constexpr size_t log_line_dist = 20;
    constexpr size_t log_amount = 320 / log_line_dist;
    constexpr size_t log_line_max_len = 96;

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

    // DisplayCtl:
    // - Controls screen overlays and stuff
    class DisplayCtl {
        enum class screen {
            DEBUG_CMD,      // Shows terminal of logs
            HOME            // Show devices connected, taskbar, buttons for config and so on.
        };

        Display* m_disp = nullptr;
        TouchCtl* m_touch = nullptr;
        screen m_screen = screen::DEBUG_CMD;
        bool m_ext_cmd_req = false;

        // param: with background on last?
        void draw_mouse(bool = true);
    public:
        void task();

        Display* get_display();

        void set_debugging();
    };

    RUN_ASYNC_ON_CORE_AUTO(DisplayCtl, DisplayTask, task, cpu_core_id_for_display, 2);
}

#include "display.ipp"