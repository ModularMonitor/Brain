#pragma once

#include "FS.h"
#include "SPI.h"
#include "TFT/TFT_eSPI.h"
#include "free_fonts.h"
#include "Serial/packaging.h"

#include <memory>

#include "devices_data_bridge.h"

namespace DP {

    enum class display_state {
        MAIN_ALL_MODULES,
        SPECIFIC_DEVICE_PLOT
    };

    class Display {
        struct Touch {
            uint16_t x, y;
            bool d = false, w = false;

            void set_down(bool);
            bool was_triggered_last_tick() const;
        };

        struct Item {
            ModuleMapping::module_data_t last_data;
            std::string last_data_str;

            const char* static_name;
            device_id self_id{};
            bool was_on = false;
            bool was_ok = true;
            bool was_sel = false;
            bool state_changed = true;

            void draw_resumed(TFT_eSPI*, const bool&, bool = false) const;
            void draw_fullscreen(TFT_eSPI*, const bool&, ModuleMapping&) const;
            void self_update(ModuleMapping&);
            bool self_check_click(Touch&);
        };

    private:
        std::unique_ptr<TFT_eSPI> tft;
        Touch m_touch;

        Item m_modules[static_cast<size_t>(device_id::_MAX)];
        Item* m_selected_module = m_modules;
        display_state m_state = display_state::MAIN_ALL_MODULES;
        uint16_t calibrationData[5];
        decltype(millis()) m_last_display_time_to_draw_ms = 0, m_last_display_time_to_draw_ms_stabilized = 0;
        bool m_had_transition = true; // true for one tick/loop
        
        /*  == AUX FUNCTIONS ==  */
        void _print_item_as_list_and_store_if_clicked(const char*, const bool, const bool, size_t);

        /*  == HELP FUNCTIONS ==  */
        void print_top_part(const bool&);
        void print_bottom_part();
    public:    
        Display(const Display&) = delete;
        Display(Display&&) = delete;
        void operator=(const Display&) = delete;
        void operator=(Display&&) = delete;
        Display();

        void task();
    };

    RUN_ASYNC_ON_CORE_AUTO(Display, DisplayTask, task, cpu_core_id_for_display, 2);
}

#include "display.ipp"