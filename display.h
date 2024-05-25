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
        bool m_is_log_screen = true;
    public:
        Display();

        TFT_eSPI* operator->();
        const TFT_eSPI* operator->() const;

        bool is_debugging() const; // check on DisplayCtl to see if thread should call terminal_print() for debugging or not!
        void toggle_debugging(); // hook to button!
        void set_debugging(const bool); // forced

        void terminal_print();
    };

    // TouchCtl
    // - Hook stuff to events on touch release
    class TouchCtl {
        std::shared_ptr<TFT_eSPI> m_tft;
    public:

    };

    // DisplayCtl:
    // - Controls screen overlays and stuff
    class DisplayCtl {
        Display* m_disp = nullptr;
    public:
        void task();

        Display* get_display();
    };

    RUN_ASYNC_ON_CORE_AUTO(DisplayCtl, DisplayTask, task, cpu_core_id_for_display, 2);



//    enum class display_state {
//        MAIN_ALL_MODULES,
//        SPECIFIC_DEVICE_PLOT
//    };
//
//    class Display {
//        struct Touch {
//            uint16_t x = 0, y = 0;
//            bool d = false, w = false;
//
//            void set_down(bool);
//            bool was_triggered_last_tick() const;
//        };
//
//        struct Item {
//            STR::StoredDataEachDevice* m_ref = nullptr;
//            
//            const STR::RepresentedData* m_sel = nullptr;
//            const STR::RepresentedData::data_storage* m_dat = nullptr;
//
//            const char* static_name;
//            CS::device_id self_id{};
//            bool was_on = false;
//            bool was_ok = true;
//            bool was_sel = false;
//            bool state_changed = true;
//
//            void draw_resumed(TFT_eSPI*, const bool&, bool = false) const;
//            void draw_fullscreen(TFT_eSPI*, const bool&) const;
//            void self_update();
//            bool self_check_click(Touch&);
//        };
//
//    private:
//        std::unique_ptr<TFT_eSPI> tft;
//        Touch m_touch;
//
//        Item m_modules[static_cast<size_t>(CS::device_id::_MAX)];
//        Item* m_selected_module = m_modules;
//
//        display_state m_state = display_state::MAIN_ALL_MODULES;
//        uint16_t calibrationData[5];
//        uint64_t m_last_display_time_to_draw_ms = 0, m_last_display_time_to_draw_ms_stabilized = 0;
//        bool m_had_transition = true; // true for one tick/loop
//        bool m_time_change_helper = false; // used in some in-screen by-time changes
//        
//        /*  == AUX FUNCTIONS ==  */
//        void _print_item_as_list_and_store_if_clicked(const char*, const bool, const bool, size_t);
//
//        /*  == HELP FUNCTIONS ==  */
//        void print_top_part(const bool&);
//        void print_bottom_part();
//    public:    
//        Display(const Display&) = delete;
//        Display(Display&&) = delete;
//        void operator=(const Display&) = delete;
//        void operator=(Display&&) = delete;
//        Display();
//
//        void task();
//    };
//
//    RUN_ASYNC_ON_CORE_AUTO(Display, DisplayTask, task, cpu_core_id_for_display, 2);
}

#include "display.ipp"