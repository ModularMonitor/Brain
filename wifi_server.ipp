#pragma once

#include "wifi_server.h"
#include "wifi_server_pages.h"

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
static const String myHostname = "modularmonitor.local";

static const IPAddress apIP(8, 8, 8, 8);
static const IPAddress netMsk(255, 255, 255, 0);
constexpr byte DNS_PORT = 53;

inline void __handle_wifi_event(const wifi_page_endpoints& ev)
{
    auto& wifi = GET(MyWiFiPortal);
    if (!wifi.m_build) {
        LOGE(e_LOG_TAG::TAG_WIFI, "WiFi event handler being called without a WiFi set up! How? Unhandled!");
        return;
    }

    const auto isIp = [](String str) {
        for (size_t i = 0; i < str.length(); i++) {
            int c = str.charAt(i);
            if (c != '.' && (c < '0' || c > '9')) {
            return false;
            }
        }
        return true;
    };
    const auto toStringIp = [](IPAddress ip) {
        String res = "";
        for (int i = 0; i < 3; i++) {
            res += String((ip >> (8 * i)) & 0xFF) + ".";
        }
        res += String(((ip >> 8 * 3)) & 0xFF);
        return res;
    };
    const auto redirectAuto = [&] {
        wifi.m_build->server.sendHeader("Location", String("http://") + myHostname, true);
        wifi.m_build->server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
        wifi.m_build->server.client().stop(); // Stop is needed because we sent no content length
    };
    const auto captivePortal = [&] {
        if (!isIp(wifi.m_build->server.hostHeader()) && wifi.m_build->server.hostHeader() != myHostname) {            
            //LOGI_NOSD(e_LOG_TAG::TAG_WIFI, "Captive portal detection, redirecting...");
            
            redirectAuto();

            return true;
        }
        return false;
    };

    if (captivePortal()) return;

    switch(ev) {
    case wifi_page_endpoints::NOT_FOUND:
        //LOGI_NOSD(e_LOG_TAG::TAG_WIFI, "Requested %s, redirect...", wifi.m_build->server.hostHeader().c_str());
        redirectAuto();
        break;
    case wifi_page_endpoints::ROOT:
        //LOGI_NOSD(e_LOG_TAG::TAG_WIFI, "Requested /, replying.");
        wifi.m_build->server.send(200, "text/html", get_webserver_home());
        break;
    case wifi_page_endpoints::CSS:
        //LOGI_NOSD(e_LOG_TAG::TAG_WIFI, "Requested /css.css, replying.");
        wifi.m_build->server.send(200, "text/css", get_webserver_css());
        break;
        break;
    case wifi_page_endpoints::JS:
        //LOGI_NOSD(e_LOG_TAG::TAG_WIFI, "Requested /js.js, replying.");
        wifi.m_build->server.send(200, "text/javascript", get_webserver_js());
        break;
    case wifi_page_endpoints::DATETIME:
        //LOGI_NOSD(e_LOG_TAG::TAG_WIFI, "Requested /time, replying.");
        wifi.m_build->server.send(200, "application/json", String("{\"time\":") + String(get_time_ms()) + String("}"));
        break;
    case wifi_page_endpoints::BUILDTIME:
        //LOGI_NOSD(e_LOG_TAG::TAG_WIFI, "Requested /build, replying.");
        wifi.m_build->server.send(200, "application/json", String("{\"build\":\"") + String(__DATE__) + " " + String(__TIME__) + String("\"}"));
        break;
    case wifi_page_endpoints::DEVICECOUNT:
        //LOGI_NOSD(e_LOG_TAG::TAG_WIFI, "Requested /get_device/count, replying.");
        wifi.m_build->server.send(200, "application/json", String("{\"count\":") + String(CS::d2u(CS::device_id::_MAX)) + String("}"));
        break;
    }
}

inline void __handle_wifi_getdata(const uint8_t dev)
{
    //LOGI_NOSD(e_LOG_TAG::TAG_WIFI, "Requested data of device %i, replying.", static_cast<int>(dev));

    auto& wifi = GET(MyWiFiPortal);
    if (!wifi.m_build) {
        LOGE(e_LOG_TAG::TAG_WIFI, "WiFi event handler being called without a WiFi set up! How? Unhandled!");
        return;
    }

    auto& sv = wifi.m_build->server;

    if (dev >= CS::d2u(CS::device_id::_MAX)) { // cancel on numbers not valid
        sv.send(501, "text/plain", ""); // Not Implemented
        sv.client().stop();
        return;
    }

    const CS::device_id dev_id = static_cast<CS::device_id>(dev);
    const MyI2Ccomm& com = GET(MyI2Ccomm);
    constexpr size_t max_time_back = MyI2Ccomm::get_max_history_size();   
    const unsigned maxx = com.get_device_configurations(dev_id, 0).m_map.size();
    
    const long index = sv.hasArg("index") ? sv.arg("index").toInt() : -1;

    // show resumed only (online, has issues)
    const bool resumed = sv.hasArg("resumed") ? (sv.arg("resumed") == "true") : (index < 0 || index >= maxx);

    String json_build = "{";

    if (com.is_device_online(dev_id)) json_build += "\"online\":true,";
    else                              json_build += "\"online\":false,";

    if (com.is_device_with_issue(dev_id)) json_build += "\"has_issues\":true,";
    else                                  json_build += "\"has_issues\":false,";

    json_build += "\"max_index\":" + String(maxx) + ","
        "\"last_updated\":" + String(com.get_device_configurations(dev_id, 0).m_update_time);


    if (!resumed) {
        json_build += ",\"path\":\"" + String(com.get_device_data_in_time(dev_id, 0, static_cast<size_t>(index)).first.c_str()) +  "\""
            ",\"data\":[";

        for(int p = static_cast<int>(max_time_back) - 1; p >= 0; --p) {
            const i2c_data_pair& it = com.get_device_data_in_time(dev_id, (p + 1) % max_time_back, static_cast<size_t>(index));

            //json_build += "{\"name\":\"" + String(it.first.c_str()) + "\",\"value\":" + String(std::strtod(it.second, nullptr)) + "}";

            json_build += String(std::strtod(it.second, nullptr), 6);
            if (p > 0) json_build += ",";
        }

        json_build += "]";
    }

    json_build += "}";

    sv.send(200, "application/json", json_build);
}

std::string random_wifi_name();
std::string random_wifi_password();


inline MyWiFiPortal::ref::ref()
    : dnsServer(), server(80)
{}

inline void MyWiFiPortal::handle_requests()
{
    LOGI(e_LOG_TAG::TAG_WIFI, "WiFi handler thread started!");

    if (!m_build) {
        LOGE(e_LOG_TAG::TAG_WIFI, "WiFi handler thread got an issue. How is m_build not initialized yet? ABORT!");
        vTaskDelete(NULL);
        return;
    }

    while(1) {
        m_build->server.handleClient();
        SLEEP(25);
    }
}

inline MyWiFiPortal::MyWiFiPortal()
    //: m_ssid(random_wifi_name()), m_pass(random_wifi_password())
    : m_ssid("ESP32TEST"), m_pass("mymom123")
{
    const std::string tmp = "WIFI:T:WPA;S:" + m_ssid + ";P:" + m_pass + ";H:false;";
    m_qrcode = std::unique_ptr<qrcodegen::QrCode>(new qrcodegen::QrCode(qrcodegen::QrCode::encodeText(tmp.c_str(), qrcodegen::QrCode::Ecc::ECC_LOW)));
}

inline void MyWiFiPortal::start()
{
    if (m_thr) return;

    LOGI(e_LOG_TAG::TAG_WIFI, "Configuring WiFi Portal...");

    WiFi.softAPConfig(apIP, apIP, netMsk);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(m_ssid.c_str(), m_pass.c_str());

    m_build = std::unique_ptr<ref>(new ref());

    m_build->dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    m_build->dnsServer.start(DNS_PORT, "*", apIP);

    m_build->server.on("/",                 [] { __handle_wifi_event(wifi_page_endpoints::ROOT);         });
    m_build->server.on("/js.js",            [] { __handle_wifi_event(wifi_page_endpoints::JS);           });
    m_build->server.on("/css.css",          [] { __handle_wifi_event(wifi_page_endpoints::CSS);          });
    m_build->server.on("/time",             [] { __handle_wifi_event(wifi_page_endpoints::DATETIME);     });
    m_build->server.on("/build",            [] { __handle_wifi_event(wifi_page_endpoints::BUILDTIME);    });
    m_build->server.on("/get_device/count", [] { __handle_wifi_event(wifi_page_endpoints::DEVICECOUNT);  });

    for(uint8_t dev = 0; dev < CS::d2u(CS::device_id::_MAX); ++dev) {
        m_build->server.on(String("/get_device/") + dev, [cur = dev]() { __handle_wifi_getdata(cur); });
    }
    
    // Android fix from "https://github.com/espressif/arduino-esp32/issues/1037"
    //m_build->server.on("/generate_204", [] { __handle_wifi_event(wifi_page_endpoints::NOT_FOUND);    });
    m_build->server.onNotFound([]{ __handle_wifi_event(wifi_page_endpoints::NOT_FOUND); });

    m_build->server.begin();

    m_thr = async_class_method(MyWiFiPortal, handle_requests, cpu_core_id_for_wifi_setup);

    LOGI(e_LOG_TAG::TAG_WIFI, "Started WiFi Portal handler thread.");
}

inline void MyWiFiPortal::stop()
{
    LOGI(e_LOG_TAG::TAG_WIFI, "Stopping WiFi Portal...");
    if (m_thr) vTaskDelete(m_thr);
    m_thr = nullptr;

    m_build->server.stop();
    m_build->dnsServer.stop();

    m_build.reset();

    LOGI(e_LOG_TAG::TAG_WIFI, "Stopped WiFi Portal.");
}

inline const qrcodegen::QrCode& MyWiFiPortal::get_qr_code() const
{
    return *m_qrcode;
}

inline const std::string& MyWiFiPortal::get_ssid() const
{
    return m_ssid;
}

inline const std::string& MyWiFiPortal::get_password() const
{
    return m_pass;
}



inline std::string random_wifi_name()
{
    std::string res;

    if (esp_random() % 2 == 0) res += "fus";
    else res += "cli";

    for(size_t rc = 0; rc < 6; ++rc) res += (esp_random() % 2 == 0) ? ('A' + (esp_random() % 26)) : ('a' + (esp_random() % 26));

    return res;
}

inline std::string random_wifi_password()
{
    std::string res;

    for(size_t rc = 0; rc < 8; ++rc) {
        switch(esp_random() % 3) {
        case 0: res += ('A' + (esp_random() % 26)); break;
        case 1: res += ('a' + (esp_random() % 26)); break;
        case 2: res += ('0' + (esp_random() % 10)); break;
        }
    }

    return res;
}
