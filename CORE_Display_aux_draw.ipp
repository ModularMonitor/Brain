#pragma once

//#include "CORE_Display.h"
#include "CORE_Display_aux_draw.h"
#include "I2C_communication.h"
#include "LOG_ctl.h"

inline void DisplayLineBlock::draw_rounded_device_box(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r)
{
    m_tft->drawFastHLine(0, y, w, m_c_nodata);
    m_tft->drawFastHLine(0, y + h - 1, w, m_c_nodata);

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
    if (m_title_last.length() || m_description_last.length() || m_extra_last.length())
    {
        m_tft->setTextColor(m_c_fill, m_c_fill);
        if (m_extra_last.length()) {
            m_tft->drawString(m_title_last.c_str(),          6 + x,  5 + y, 2);
            m_tft->drawString(m_description_last.c_str(),    6 + x, 20 + y, 2);
            m_tft->drawString(m_extra_last.c_str(),          6 + x, 35 + y, 2);
        }
        else {
            m_tft->drawString(m_title_last.c_str(),          6 + x,  5 + y, 2);
            m_tft->drawString(m_description_last.c_str(),    6 + x, 30 + y, 2);
        }

        m_title_last.clear();
        m_description_last.clear();
        m_extra_last.clear();
    }

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

inline void DisplayLineBlock::set_title(const std::string& s)
{
    if (s == m_title) return;
    m_title_last = m_title;
    m_title = s;
}

inline void DisplayLineBlock::set_description(const std::string& s)
{
    if (s == m_description) return;
    m_description_last = m_description;
    m_description = s;
}

inline void DisplayLineBlock::set_extra(const std::string& s)
{
    if (s == m_extra) return;
    m_extra_last = m_extra;
    m_extra = s;
}

inline void DisplayLineBlock::set_tft(std::shared_ptr<TFT_eSPI> tft)
{
    m_tft = tft;
}

inline void DisplayLineBlock::set_texts(const char* title, const char* desc, const char* extra)
{
    set_title(title);
    set_description(desc);
    set_extra(extra);
}

inline void DisplayLineBlock::set_state_changed()
{
    m_has_changes = true;
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

inline void DisplayLineBlock::set_nodata_color(const uint16_t c)
{
    m_c_nodata = c;
}

inline void DisplayLineBlock::draw(const int32_t off_y)
{
    using namespace DisplayColors;

    const bool is_all_empty = m_title.empty() && m_description.empty() && m_extra.empty();

    //const MyI2Ccomm::device& dev_dev = GET(MyI2Ccomm).get_device_configurations(m_last_dev, 0);

    //const uint16_t fill = 
    //    dev_dev.m_issues ? item_has_issues_bg_color : (dev_dev.m_online ? item_online_bg_color : item_offline_bg_color);
    //const uint16_t border =
    //    dev_dev.m_issues ? item_has_issues_bg_color_border : (dev_dev.m_online ? item_online_bg_color_border : item_offline_bg_color_border);
    //const uint16_t font_color = TFT_BLACK;

    if (!m_was_string_empty && is_all_empty) {
        m_was_string_empty = true;
        m_has_changes = false; // avoid second draw
        m_tft->fillRect(0, bar_top_height + off_y * item_resumed_height_max, item_resumed_width_max, item_resumed_height_max, m_c_nodata);
    }
    else if (m_was_string_empty && !is_all_empty) {
        m_was_string_empty = false;
        m_has_changes = true;
    }

    if (m_has_changes) {
        draw_rounded_device_box(0, bar_top_height + off_y * item_resumed_height_max, item_resumed_width_max, item_resumed_height_max, item_resumed_border_radius);
        m_has_changes = false;
    }

    if (!is_all_empty) draw_text_auto(0, bar_top_height + off_y * item_resumed_height_max);

    // draw graph
}


inline void DisplayFullBlockGraph::update_with(CS::device_id current_dev, const unsigned current_off)
{
    using namespace DisplayColors;

    m_last_dev = current_dev;
    set_title(get_fancy_name_for(current_dev)); // TITLE SET!

    if (current_off < 0) {
        for(auto& i : m_history_in_graph) i = 0.0;
        return;
    }
    
    MyI2Ccomm& com = GET(MyI2Ccomm);
    constexpr size_t max_time_back = MyI2Ccomm::get_max_history_size();
    char buf[64];

    {
        const MyI2Ccomm::device& ref_map = com.get_device_configurations(current_dev, 0); // get newest map
        std::lock_guard<std::mutex> l(ref_map.m_map_mtx);

        if (current_off >= ref_map.m_map.size()) {
            for(auto& i : m_history_in_graph) i = 0.0;
            return;
        }
        
        ms2str(buf, sizeof(buf), get_time_ms() - ref_map.m_update_time);

        set_description(std::next(ref_map.m_map.begin(), current_off)->first); // DESC SET (as filter too)
        set_extra("Atualizado faz " + std::string(buf) + ".");
    }

    m_max = -1e80;
    m_min = 1e80;

    for(size_t p = 0; p < max_time_back; ++p) {
        const i2c_data_pair& it = com.get_device_data_in_time(current_dev, (p + 1) % max_time_back, current_off);

        if (it.first.empty()) {
            m_history_in_graph[p] = 0.0;
        }
        else {
            m_history_in_graph[p] = std::strtod(it.second, nullptr);
        }
        if (m_history_in_graph[p] > m_max) m_max = m_history_in_graph[p];
        if (m_history_in_graph[p] < m_min) m_min = m_history_in_graph[p];
    }

    double delta = (m_max - m_min);
    if (delta == 0.0) delta = 1e-50;

    // normalize to [0..1]
    for(auto& i : m_history_in_graph)
        i = (i - m_min) * 1.0 / delta;

    
}

inline void DisplayFullBlockGraph::draw()
{
    if (m_last_dev == CS::device_id::_MAX) return;

    using namespace DisplayColors;
    constexpr size_t max_time_back = MyI2Ccomm::get_max_history_size();

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
    m_tft->fillRect(graph_margin_left, graph_margin_top, graph_width_calculated, graph_height_calculated, graph_background_color);

    constexpr float factor_x = static_cast<float>(graph_width_calculated) / max_time_back;
    constexpr float factor_y = static_cast<float>(graph_height_calculated);

    for(size_t p = 0; p < (max_time_back - 1); ++p)
    {
        const int32_t curr_st_x = (static_cast<float>(p) * factor_x) + graph_margin_left;
        const int32_t curr_en_x = (static_cast<float>(p + 1) * factor_x) + graph_margin_left;
        const int32_t curr_st_y = static_cast<float>(0.99 - 0.98 * m_history_in_graph[p]) * factor_y + graph_margin_top;
        const int32_t curr_en_y = static_cast<float>(0.99 - 0.98 * m_history_in_graph[p + 1]) * factor_y + graph_margin_top;

        //LOGI_NOSD(e_LOG_TAG::TAG_CORE, "%.4lf -> %.4lf", m_history_in_graph[p], m_history_in_graph[p+1]);

        m_tft->drawLine(curr_st_x, curr_st_y, curr_en_x, curr_en_y, graph_line_color);
    }

    char buf[64];

    snprintf(buf, sizeof(buf), "       %04.2lf", m_max);    
    auto off_left = m_tft->textWidth(buf, 2) + 2;
    m_tft->drawString(buf, graph_margin_left - off_left, graph_margin_top - 5, 2);

    snprintf(buf, sizeof(buf), "        %04.2lf", m_min);
    off_left = m_tft->textWidth(buf, 2) + 2;
    m_tft->drawString(buf, graph_margin_left - off_left, graph_margin_top + graph_height_calculated - 5, 2);
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