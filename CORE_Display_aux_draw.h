#pragma once

#include "SD_card.h"
#include "Custom_Bitmaps.h"
#include "Configuration.h"

#include "TFT/TFT_eSPI.h"

// A 40px liner with info
class DisplayLineBlock {
protected:
    std::string m_title, m_description, m_extra;
    std::string m_title_last, m_description_last, m_extra_last; // using this to print with fill color and clean after use    

    uint16_t m_c_fill, m_c_border, m_c_font, m_c_nodata;
    bool m_has_changes = true, m_was_string_empty = false;

    std::shared_ptr<TFT_eSPI> m_tft;

    void draw_rounded_device_box(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r);
    void draw_text_auto(int32_t x, int32_t y);

    void set_title(const std::string&);
    void set_description(const std::string&);
    void set_extra(const std::string&);
public:
    void set_tft(std::shared_ptr<TFT_eSPI>);

    void set_texts(const char*, const char*, const char*);
    void set_state_changed();

    void set_fill_color(const uint16_t);
    void set_border_color(const uint16_t);
    void set_font_color(const uint16_t);
    void set_nodata_color(const uint16_t);

    void draw(const int32_t off_y);
};

class DisplayFullBlockGraph : public DisplayLineBlock {
    double m_history_in_graph[i2c_values_history_size]{0.0}; // normalized to [0..1]
    double m_max = 0.0, m_min = 0.0;
    uint64_t m_hist_last = -1.0;
    bool m_hist_should_redraw = true;

    CS::device_id m_last_dev = CS::device_id::_MAX;

    using DisplayLineBlock::set_texts; // private it. Use update_with instead
public:
    using DisplayLineBlock::set_state_changed;
    using DisplayLineBlock::set_fill_color;
    using DisplayLineBlock::set_border_color;
    using DisplayLineBlock::set_font_color;
    using DisplayLineBlock::set_nodata_color;

    void update_with(CS::device_id current_dev, const unsigned current_off);

    void draw();
};