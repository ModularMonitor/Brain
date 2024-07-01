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

    inline void SIM::update_test_send_tcp()
    {
        const auto curr_ip = m_modem->getLocalIP();
        LOGI(TAG, "Current IP: %s.", curr_ip.c_str());

        const int port = 80;
        constexpr char* server = "ipv6-test.com";
        constexpr char* resource = "/api/myip.php";
        auto m_connection = std::unique_ptr<TinyGsmClient>(new TinyGsmClient(*m_modem));

        if (!m_connection->connect(server, port)) {
            LOGE(TAG, "Could not connect to '%s'", server);
            return;
        } else {
            // Make a HTTP GET request:
            m_connection->print(String("GET ") + resource + " HTTP/1.1\r\n");
            m_connection->print(String("Host: ") + server + "\r\n");
            m_connection->print("Connection: close\r\n\r\n");

            // Wait for data to arrive
            uint32_t start = millis();

            while (m_connection->connected() && !m_connection->available() && millis() - start < 30000L)
            {
                delay(100);
            }

            // Read data
            start = millis();
            char data[512]{};

            size_t read_chars = 0;
            while (m_connection->connected() && millis() - start < 10000L) {
                while (m_connection->available()) {
                    data[read_chars++] = m_connection->read();
                    start = millis();
                }
            }

            LOGI(TAG, "GOT information!");

            for(size_t p = 0; p < read_chars; ++p) {
                Serial.printf("%c", data[p]);
            }
       
            
            //m_connection->stop();
        }




//        constexpr auto tcp_port = 40302;
//        constexpr char* url_path = "lohks.v6.army";
//        constexpr size_t tcp_number_max_len = 8; // 12345678
//        auto m_connection = std::unique_ptr<TinyGsmClient>(new TinyGsmClient(*m_modem));
//
//        if (!m_connection->connect(url_path, tcp_port)) {
//            LOGE(TAG, "Could not connect to '%s'", url_path);
//            return;
//        }
//
//        LOGI(TAG, "Connected to '%s'!!! That's nice!", url_path);
//
//        const auto send_number_through = [&](const int num)
//        {
//            char buf[tcp_number_max_len + 1];
//            snprintf(buf, sizeof(buf), "%i", num);
//            m_connection->write((uint8_t*)buf, tcp_number_max_len);
//        };
//
//        const auto get_number_from = [&]() -> int
//        {
//            char buf[tcp_number_max_len + 1]{};
//            m_connection->readBytes(buf, tcp_number_max_len);            
//            return static_cast<int>(std::strtol(buf, nullptr, 10));
//        };
//
//        const std::string count = std::to_string(CPU::get_time_ms());
//
//        send_number_through(15);
//        send_number_through((int)count.length());
//        
//        m_connection->write((uint8_t*)count.c_str(), count.length());
//
//        delay(500);
//
//        LOGI(TAG, "Sent data to %s. Closing.", url_path);
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
            m_modem = std::unique_ptr<TinyGsm>(new TinyGsm(SerialAT));
            m_modem->restart();
            String modeminfo = m_modem->getModemInfo();
            LOGI(TAG, "Started modem '%s'. Waiting for Network now...", modeminfo.c_str());
            m_modem->waitForNetwork(600000L);
            LOGI(TAG, "Connecting to GPRS...");
            m_modem->gprsConnect(apn);
            //const String time_now = m_modem->getGSMDateTime(TinyGSMDateTimeFormat::DATE_FULL);
            update_location();
            update_rssi();
            //update_test_send_tcp(); // does not work ipv6

            LOGI(TAG, "Got time: %04i/%02i/%02i %02i:%02i:%02i",
                m_loc.tm_year + 1900, m_loc.tm_mon + 1, m_loc.tm_mday,
                m_loc.tm_hour, m_loc.tm_min, m_loc.tm_sec);
        }

        if (m_loc_upd.is_time()) update_location();
        if (m_rssi_upd.is_time()) update_rssi();
        //if (m_refresh_tcp_upd.is_time()) update_test_send_tcp(); // does not work ipv6
    }

}