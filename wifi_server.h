#pragma once

#include "qr_code.h"
#include "defaults.h"
#include "LOG_ctl.h"
#include "I2C_communication.h"

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

enum class wifi_page_endpoints {
    ROOT,               // base
    CSS,                // css file
    JS,                 // js file
    NOT_FOUND,          // 404
    DATETIME,           // just experiments
    BUILDTIME,          // show build time
    DEVICECOUNT,        // device_id::MAX
};


void __handle_wifi_event(const wifi_page_endpoints& ev);
void __handle_wifi_getdata(const uint8_t dev);

MAKE_SINGLETON_CLASS(MyWiFiPortal, {
    const std::string m_ssid;
    const std::string m_pass;
    std::unique_ptr<qrcodegen::QrCode> m_qrcode;
    TaskHandle_t m_thr = nullptr;

    struct ref {
        DNSServer dnsServer;
        WebServer server;

        ref();
    };

    std::unique_ptr<ref> m_build;

    friend void __handle_wifi_event(const wifi_page_endpoints& ev);
    friend void __handle_wifi_getdata(const uint8_t dev);

    void handle_requests();
public:
    MyWiFiPortal();

    void start();
    void stop();

    const qrcodegen::QrCode& get_qr_code() const;
    const std::string& get_ssid() const;
    const std::string& get_password() const;
});

#include "wifi_server.ipp"
