#pragma once

#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

namespace LG {
    
    inline Logger::Logger()
    {
        memset(m_lines_buffering, 0, log_line_max_len * log_amount);
    }

    inline int Logger::printf(const char* tag, const char* format, ...)
    {
        std::lock_guard<std::mutex> l(m_mtx);

        try {
            for(size_t p = log_amount - 1; p != 0; --p)
                strncpy(m_lines_buffering[p], m_lines_buffering[p - 1], log_line_max_len);

            //memset(m_lines_buffering[0], '\0', log_line_max_len);

            int written = snprintf(m_lines_buffering[0], log_line_max_len, "[#%i]%s: ", (int)xPortGetCoreID(), tag);
            if (written < 0) written = 0;
            if (written >= log_line_max_len) return 0;

            va_list args;
            va_start (args, format);
            const int res = vsnprintf(m_lines_buffering[0] + written, log_line_max_len - written, format, args);
            va_end(args);
            
            if (Serial) {
                Serial.printf("[CPU%i][%s][I] ", (int)xPortGetCoreID(), tag);
                Serial.printf("%.*s", res, m_lines_buffering[0] + written);
                Serial.println();
            }
            m_had_news = true;

            return res;
        }
        catch(const std::exception& e)
        {
            return snprintf(m_lines_buffering[0], log_line_max_len, "[#%i]%s EXC: %s", (int)xPortGetCoreID(), tag, e.what());
        }
        m_lines_buffering[0][log_line_max_len-1] = '\0';
    }

    inline const char* Logger::get_line(const size_t i) const
    {
        if (i >= log_amount) return "";
        return m_lines_buffering[i];
    }

    inline size_t Logger::size() const
    {
        return log_amount;
    }

    inline bool Logger::for_display_had_news()
    {
        return EXC_RETURN(m_had_news, false);
    }

}
