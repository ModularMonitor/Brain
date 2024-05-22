#pragma once

#include "FS.h"
#include "SPI.h"
#include "TFT/TFT_eSPI.h"
#include "free_fonts.h"
#include "Serial/packaging.h"
#include "devices_data_bridge.h"
#include "cpu_manager.h"
#include "sdcard.h"
#include <memory>

/*
Format of the screen:
20 px y = top informative bar
40... = cards
20 last pixels = debug
*/

#define CALIBRATION_FILE "/calibrationData"

inline std::string ensureN(std::string input, size_t last_n) { 
    return last_n > input.length() ? (input + std::string(last_n - input.length(), ' ')) : input.substr(input.size() - last_n); 
}

class Display {
public:
    enum class display_state {
        MAIN_ALL_MODULES,
        SPECIFIC_DEVICE_PLOT
    };

    struct last_touch {
        uint16_t x = 0, y = 0;
        bool down = false, last_was_down = false;
        void set_down(bool d) {last_was_down = down; down = d; }
        bool was_triggered_last_tick() const { return last_was_down && !down; }
    } m_touch;

    struct item_list {
        ModuleMapping::module_data_t last_data;
        std::string last_data_str;

        const char* static_name;
        device_id self_id{};
        bool was_on = false;
        bool was_ok = true;
        bool was_sel = false;
        bool state_changed = true;

        void self_draw_resume(TFT_eSPI*, const bool&, bool = false) const;
        void self_draw_alone(TFT_eSPI*, const bool&, ModuleMapping&) const;
        void self_update(ModuleMapping&);
        bool self_check_click(last_touch&);

    };
    
private:
    std::unique_ptr<TFT_eSPI> tft;

    item_list m_modules[static_cast<size_t>(device_id::_MAX)];
    item_list* m_selected_module = m_modules;
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

    void think_and_draw();
};
