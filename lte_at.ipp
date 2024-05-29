#pragma once

#include "lte_at.h"

#define SerialAT Serial1

namespace LTE {

    inline void SIM::update_location()
    {
        float timezone = 0.0f;
        m_modem->getNetworkTime(&m_loc.tm_year, &m_loc.tm_mon, &m_loc.tm_mday, &m_loc.tm_hour, &m_loc.tm_min, &m_loc.tm_sec, &timezone);
        m_loc.tm_mon--;
        m_loc.tm_year -= 1900;
        int timezone_hours = static_cast<int>(timezone);
        int timezone_mins = static_cast<int>(timezone * 60.0f) % 60;
        m_loc.tm_hour += timezone_hours;
        m_loc.tm_min += timezone_mins;
        mktime(&m_loc);

        STR::SharedData& shr = STR::get_singleton_of_SharedData();
        shr.get_sim_data().set_time(m_loc);
    }

    inline void SIM::update_rssi()
    {
        int raw_rssi = m_modem->getSignalQuality();
        
        switch(raw_rssi) {
        case 0: raw_rssi = -113; break; // or less
        case 1: raw_rssi = -111; break;
        case 31: raw_rssi = -51; break; // or greater
        case 99: raw_rssi = -1; break; // unknown
        case 100: raw_rssi = -116; break; // or less
        case 101: raw_rssi = -115; break;
        case 191: raw_rssi = -25; break; // or greater
        case 199: raw_rssi = -1; break; // unknown
        default:
            if (raw_rssi >= 2 && raw_rssi <= 30) raw_rssi = map(raw_rssi, 2, 30, -109, -53);
            else if (raw_rssi >= 102 && raw_rssi < 191) raw_rssi = map(raw_rssi, 102, 191, -114, -26);
            else raw_rssi = -1;
            break;
        }

        m_rssi = raw_rssi;
        STR::SharedData& shr = STR::get_singleton_of_SharedData();
        shr.get_sim_data().set_rssi(m_rssi);
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

}