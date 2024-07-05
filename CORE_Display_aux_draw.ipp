#pragma once

//#include "CORE_Display.h"
#include "CORE_Display_aux_draw.h"
#include "I2C_communication.h"

inline const char* get_fancy_name_for(CS::device_id id)
{
    switch(id) {
    case CS::device_id::DHT22_SENSOR:       return "Modulo de temperatura e umidade";
    case CS::device_id::MICS_6814_SENSOR:   return "Modulo de NO2, NH3 e CO";
    case CS::device_id::LY038_HW072_SENSOR: return "Modulo de som e luz";
    case CS::device_id::GY87_SENSOR:        return "Modulo acelerometro, giroscopio, barometro";
    case CS::device_id::CCS811_SENSOR:      return "Modulo de eCO2 e TVOC";
    case CS::device_id::PMSDS011_SENSOR:    return "Modulo de particulas no ar";
    case CS::device_id::BATTERY_SENSOR:     return "Modulo da bateria";
    default:                                return "Desconhecido";
    }
}

inline void DisplayLineBlock::draw_rounded_device_box(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r)
{
    m_tft->fillRoundRect(
        1, 1 + y, // start x y
        w - 2, h - 2, r, // end x y
        m_c_border);
    m_tft->fillRoundRect(
        3, 3 + y, // start x y
        w - 6, h - 6, r, // end x y
        m_c_fill);
}

inline void DisplayLineBlock::draw_text_auto(int32_t x, int32_t y)
{
    m_tft->setTextColor(m_c_font, m_c_fill);

    if (m_extra.length()) {
        m_tft->drawString(m_title.c_str(),          6 + x,  5 + y, 2);
        m_tft->drawString(m_description.c_str(),    6 + x, 20 + y, 2);
        m_tft->drawString(m_extra.c_str(),          6 + x, 35 + y, 2);
    }
    else {
        m_tft->drawString(m_title.c_str(),          6 + x,  5 + y, 2);
        m_tft->drawString(m_description.c_str(),    6 + x, 30 + y, 2);
    }
}

inline void DisplayLineBlock::set_tft(std::shared_ptr<TFT_eSPI> tft)
{
    m_tft = tft;
}

inline void DisplayLineBlock::update_should_draw_fully(const bool v)
{
    m_has_changes |= v;
}

inline void DisplayLineBlock::set_texts(const char* title, const char* desc, const char* extra)
{
    if (title) m_title = title;
    else m_title.clear();

    if (desc) m_description = desc;
    else m_description.clear();

    if (extra) m_extra = extra;
    else m_extra.clear();
}

inline void DisplayLineBlock::set_fill_color(const uint16_t c)
{
    m_c_fill = c;
}

inline void DisplayLineBlock::set_border_color(const uint16_t c)
{
    m_c_border = c;
}

inline void DisplayLineBlock::set_font_color(const uint16_t c)
{
    m_c_font = c;
}

inline void DisplayLineBlock::draw(const int32_t off_y)
{
    using namespace DisplayColors;

    //const MyI2Ccomm::device& dev_dev = GET(MyI2Ccomm).get_device_configurations(m_last_dev, 0);

    //const uint16_t fill = 
    //    dev_dev.m_issues ? item_has_issues_bg_color : (dev_dev.m_online ? item_online_bg_color : item_offline_bg_color);
    //const uint16_t border =
    //    dev_dev.m_issues ? item_has_issues_bg_color_border : (dev_dev.m_online ? item_online_bg_color_border : item_offline_bg_color_border);
    //const uint16_t font_color = TFT_BLACK;

    if (m_has_changes) {
        m_has_changes = false;
        draw_rounded_device_box(0, bar_top_height + off_y * item_resumed_height_max, item_resumed_width_max, item_resumed_height_max, item_resumed_border_radius);
    }

    draw_text_auto(0, bar_top_height + off_y * item_resumed_height_max);

    // draw graph
}


inline void DisplayFullBlockGraph::update_with(CS::device_id current_dev, const int current_off)
{
    m_last_dev = current_dev;
    m_title = get_fancy_name_for(current_dev); // TITLE SET!

    if (current_off < 0) {
        for(auto& i : m_history_in_graph) i = 0.0;
        return;
    }
    
    MyI2Ccomm& com = GET(MyI2Ccomm);
    constexpr size_t max_time_back = MyI2Ccomm::get_max_history_size();

    {
        const MyI2Ccomm::device& ref_map = com.get_device_configurations(current_dev, 0); // get newest map
        std::lock_guard<std::mutex> l(ref_map.m_map_mtx);

        if (current_off >= ref_map.m_map.size()) {
            for(auto& i : m_history_in_graph) i = 0.0;
            return;
        }

        m_description = std::next(ref_map.m_map.begin(), current_off)->first; // DESC SET (as filter too)
        m_extra = "Atualizado faz " + std::to_string(get_time_ms() - ref_map.m_update_time) + " seg(s).";
    }

    double max_found = 1e-10; // max_found must be > 0 this way.
    double min_found = 0.0;

    for(size_t p = 0; p < max_time_back; ++p) {
        const std::optional<i2c_data_pair> i = com.get_device_data_in_time(current_dev, p, m_description);
        
        if (!i) {
            m_history_in_graph[p] = 0.0;
        }
        else {
            i2c_data_pair it = *i;
            m_history_in_graph[p] = std::strtod(it.second, nullptr);
            if (m_history_in_graph[p] > max_found) max_found = m_history_in_graph[p];
            if (m_history_in_graph[p] < min_found) min_found = m_history_in_graph[p];
        }
    }

    // normalize to [0..1]
    for(auto& i : m_history_in_graph)
        i = (i - min_found) / max_found;
}

inline void DisplayFullBlockGraph::draw()
{
    if (m_last_dev == CS::device_id::_MAX) return;

    using namespace DisplayColors;

    //const MyI2Ccomm::device& dev_dev = GET(MyI2Ccomm).get_device_configurations(m_last_dev, 0);

    //const uint16_t fill = 
    //    dev_dev.m_issues ? item_has_issues_bg_color : (dev_dev.m_online ? item_online_bg_color : item_offline_bg_color);
    //const uint16_t border =
    //    dev_dev.m_issues ? item_has_issues_bg_color_border : (dev_dev.m_online ? item_online_bg_color_border : item_offline_bg_color_border);
    //const uint16_t font_color = TFT_BLACK;

    if (m_has_changes) {
        m_has_changes = false;
        draw_rounded_device_box(0, bar_top_height, item_full_width_max, item_full_height_max, item_resumed_border_radius);
    }

    draw_text_auto(0, bar_top_height);

    // draw graph
}


/*inline void draw_device_detailed_info(std::shared_ptr<TFT_eSPI>& m_tft, const CS::device_id& dev, const int& off)
{
    using namespace DisplayColors;


    // used on self control for draws. This way the info is stored here, not on a generic place.
    struct __self_state_ctl {
        CS::device_id last_dev = CS::device_id::_MAX;
        int last_off = -1;
        uint16_t last_fill = 0;
        uint16_t last_border = 0;

        bool has_changes(CS::device_id d, int o, uint16_t b0, uint16_t b1) {
            const bool res = (d != last_dev || o != last_off || b0 != last_fill || b1 != last_border);
            *this = {d, 0, b0, b1};
            return res;
        }
    };
    static __self_state_ctl _ctl{};


    const char* dev_name = get_fancy_name_for(dev);
    const MyI2Ccomm::device& dev_dev = GET(MyI2Ccomm).get_device_configurations(m_device_select, 0);

    const uint16_t fill = 
        dev_dev.m_issues ? item_has_issues_bg_color : (dev_dev.m_online ? item_online_bg_color : item_offline_bg_color);
    const uint16_t border =
        dev_dev.m_issues ? item_has_issues_bg_color_border : (dev_dev.m_online ? item_online_bg_color_border : item_offline_bg_color_border);
    const uint16_t font_color = TFT_BLACK;

    const bool has_changes = _ctl.has_changes(dev, off, fill, border);

    const int32_t base_y = static_cast<int32_t>(21 + (off * item_resumed_height_max));

    if (has_changes) {
        m_tft->fillRoundRect(
            1, base_y,
            item_resumed_width_max - 2, item_resumed_height_max - 2, item_resumed_border_radius,
            border);
        m_tft->fillRoundRect(
            3, base_y + 2,
            item_resumed_width_max - 6, item_resumed_height_max - 6, item_resumed_border_radius,
            fill);
    }


}*/