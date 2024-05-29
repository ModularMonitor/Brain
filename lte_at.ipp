#pragma once

#include "lte_at.h"

#define SerialAT Serial1

namespace LTE {

    inline void SIM::update_location()
    {
        // not working right now.
        //LOGI(TAG, "Updating local time and globe position... %s",
        //    m_modem->getGsmLocation(
        //        &m_loc.lat, &m_loc.lon, &m_loc.acc,
        //        &m_loc.year, &m_loc.month, &m_loc.day,
        //        &m_loc.hour, &m_loc.minute, &m_loc.second) ? "Done." : "Failed!"
        //    );
        
        //const String time_now = m_modem->getGSMDateTime(TinyGSMDateTimeFormat::DATE_FULL);

        float timezone = 0.0f;
        m_modem->getNetworkTime(&m_loc.tm_year, &m_loc.tm_mon, &m_loc.tm_mday, &m_loc.tm_hour, &m_loc.tm_min, &m_loc.tm_sec, &timezone);
        m_loc.tm_mon--;
        m_loc.tm_year -= 1900;
        int timezone_hours = static_cast<int>(timezone);
        int timezone_mins = static_cast<int>(timezone * 60.0f) % 60;
        m_loc.tm_hour += timezone_hours;
        m_loc.tm_min += timezone_mins;
        mktime(&m_loc);
    }

    inline void SIM::update_rssi()
    {
        m_rssi = m_modem->getSignalQuality();
        //LOGI(TAG, "Updated RSSI: %i", m_rssi);
    }

    inline void SIM::task()
    {
        // Setup:
        if (!m_modem) {
            LOGI(TAG, "Initializing modem...");
            /*
            MODEM_PWRKEY IO:4 The power-on signal of the modulator must be given to it,
            otherwise the modulator will not reply when the command is sent
            */
            pinMode(MODULE4G_K, OUTPUT);
            digitalWrite(MODULE4G_K, HIGH);
            delay(300); //Need delay
            digitalWrite(MODULE4G_K, LOW);

            pinMode(MODULE4G_S, OUTPUT);
            digitalWrite(MODULE4G_S, HIGH);

            SerialAT.begin(115200, SERIAL_8N1, MODULE4G_T, MODULE4G_R);
            m_modem = new TinyGsm(SerialAT);
            m_modem->restart();
            String modeminfo = m_modem->getModemInfo();
            LOGI(TAG, "Started modem '%s'. Waiting for Network now...", modeminfo.c_str());
            m_modem->waitForNetwork(600000L);
            LOGI(TAG, "Connecting to GPRS...");
            m_modem->gprsConnect(apn);
            //const String time_now = m_modem->getGSMDateTime(TinyGSMDateTimeFormat::DATE_FULL);
            update_location();
            update_rssi();            

            LOGI(TAG, "Got time: %04i/%02i/%02i %02i:%02i:%02i",
                m_loc.tm_year + 1900, m_loc.tm_mon + 1, m_loc.tm_mday,
                m_loc.tm_hour, m_loc.tm_min, m_loc.tm_sec);
        }

        if (m_loc_upd.is_time()) update_location();
        if (m_rssi_upd.is_time()) update_rssi();
    }

    inline struct tm SIM::get_time() const
    {
        return m_loc;
    }

    inline void SIM::get_time_str(char* buf, const size_t buf_len, const SIM::time_format format, const SIM::time_type type) const
    {
        switch(format) {
        case SIM::time_format::CLOCK_FULL:
            if (type == SIM::time_type::HOUR_12) {
                snprintf(buf, buf_len, "%02i:%02i:%02i %s",
                    (m_loc.tm_hour % 12 == 0 ? 12 : m_loc.tm_hour % 12), m_loc.tm_min, m_loc.tm_sec, m_loc.tm_hour >= 12 ? "PM" : "AM");
            }
            else {
                snprintf(buf, buf_len, "%02i:%02i:%02i",
                    m_loc.tm_hour, m_loc.tm_min, m_loc.tm_sec);
            }
            break;
        case SIM::time_format::CLOCK_RESUMED:
            if (type == SIM::time_type::HOUR_12) {
                snprintf(buf, buf_len, "%02i:%02i %s",
                    (m_loc.tm_hour % 12 == 0 ? 12 : m_loc.tm_hour % 12), m_loc.tm_min, m_loc.tm_hour >= 12 ? "PM" : "AM");
            }
            else {
                snprintf(buf, buf_len, "%02i:%02i",
                    m_loc.tm_hour, m_loc.tm_min);
            }
            break;
        case SIM::time_format::DATE:
            snprintf(buf, buf_len, "%04i/%02i/%02i", m_loc.tm_year + 1900, m_loc.tm_mon + 1, m_loc.tm_mday);
            break;
        case SIM::time_format::BOTH_FULL:
            if (type == SIM::time_type::HOUR_12) {
                snprintf(buf, buf_len, "%04i/%02i/%02i %02i:%02i:%02i %s",
                    m_loc.tm_year + 1900, m_loc.tm_mon + 1, m_loc.tm_mday,
                    (m_loc.tm_hour % 12 == 0 ? 12 : m_loc.tm_hour % 12), m_loc.tm_min, m_loc.tm_sec, m_loc.tm_hour >= 12 ? "PM" : "AM");
            }
            else {
                snprintf(buf, buf_len, "%04i/%02i/%02i %02i:%02i:%02i",
                    m_loc.tm_year + 1900, m_loc.tm_mon + 1, m_loc.tm_mday,
                    m_loc.tm_hour, m_loc.tm_min, m_loc.tm_sec);
            }
            break;
        }
    }

}