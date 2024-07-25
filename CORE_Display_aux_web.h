#pragma once

#include "qr_code.h"
#include "dns_server.h"

extern "C" {
    #include <string.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"
    #include "esp_event.h"
    #include "esp_log.h"

    #include "nvs_flash.h"
    #include "esp_wifi.h"
    #include "esp_netif.h"
    #include "lwip/inet.h"

    #include "esp_http_server.h"
}

#include <string>
#include <vector>
#include <algorithm>

constexpr char root_start[] = "Hello";
constexpr size_t root_len = std::size(root_start);

struct wifi_info {
    bool event_active = false;
    uint8_t connected_devices = 0;
    std::string qrcoder;
    std::string ssid_cpy, pw_cpy;
    httpd_handle_t webserv = nullptr;
};

struct wifi_handler {
    httpd_uri_t handl = { .uri = "/", .method = HTTP_GET, .handler = nullptr, .user_ctx = nullptr };
};

struct wifi_pair {
    std::string path{};
    httpd_method_t method = HTTP_GET;

    wifi_pair() = default;
    wifi_pair(std::string&& s, httpd_method_t m) : path(std::move(s)), method(m) {}
    bool operator==(const wifi_pair& w) const { return path == w.path && method == w.method; }
    bool operator!=(const wifi_pair& w) const { return path != w.path || method != w.method; }
};


void __custom_wifi_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);


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

inline wifi_info& get_singleton_wifi_info() {
    static wifi_info wi;
    return wi;
}

inline std::vector<std::pair<wifi_pair, wifi_handler>>& get_hooks_wifi_singleton()
{
    static std::vector<std::pair<wifi_pair, wifi_handler>> hooks_wifi;
    return hooks_wifi;
}


// HTTP GET Handler
inline esp_err_t root_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAGWIFI, "Serve root");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, root_start, root_len);
    return ESP_OK;
}
// HTTP Error (404) Handler - Redirects all requests to the root page
inline esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    // Set status
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redirect to the "/" root directory
    httpd_resp_set_hdr(req, "Location", "/");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);
    ESP_LOGI(TAGWIFI, "Redirecting to root");
    return ESP_OK;
}

inline void custom_wifi_del_handle(std::string path, httpd_method_t met)
{
    auto& hooks_wifi = get_hooks_wifi_singleton();
    auto& _wifi = get_singleton_wifi_info();

    wifi_pair gen{std::move(path), met };

    if (auto it = std::find_if(hooks_wifi.begin(), hooks_wifi.end(), [&](const std::pair<wifi_pair, wifi_handler>& p) { return p.first == gen; }); it != hooks_wifi.end()) {
        httpd_unregister_uri_handler(_wifi.webserv, gen.path.c_str(), gen.method);
        hooks_wifi.erase(it);
    }
}

inline bool custom_wifi_add_handle(std::string path, httpd_method_t met, esp_err_t (*handler)(httpd_req_t*), void* usrdata = nullptr)
{
    auto& hooks_wifi = get_hooks_wifi_singleton();
    auto& _wifi = get_singleton_wifi_info();
    
    wifi_pair gen{ std::move(path), met };

    custom_wifi_del_handle(gen.path, gen.method);

    auto iit = hooks_wifi.insert(hooks_wifi.end(), std::pair<wifi_pair, wifi_handler>{ gen, wifi_handler{} }); // iterator

    //httpd_uri_t handl;// = { .uri = "/", .method = HTTP_GET, .handler = root_get_handler, .user_ctx = nullptr };
    iit->second.handl.uri = iit->first.path.c_str();
    iit->second.handl.method = met;
    iit->second.handl.handler = handler;
    iit->second.handl.user_ctx = usrdata;
    return httpd_register_uri_handler(_wifi.webserv, &iit->second.handl) == ESP_OK;
}

inline void custom_wifi_start(const std::string& sss = random_wifi_name(), const std::string& pw = random_wifi_password())
{
    ESP_LOGI(TAGWIFI, "custom_wifi_start called with '%s' '%s'", sss.c_str(), pw.c_str());
    constexpr bool autohttpfp = true;
    constexpr uint8_t maxdevices = 4;
    constexpr uint8_t chh = 9;

    auto& _wifi = get_singleton_wifi_info();

    if (_wifi.event_active) return;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      if (ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_flash_erase()) != ESP_OK) return;
      ret = nvs_flash_init();
    }
    if (ESP_ERROR_CHECK_WITHOUT_ABORT(ret) != ESP_OK) return;

    if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_init()) != ESP_OK) return;
    if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_loop_create_default()) != ESP_OK) return;
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_init(&cfg)) != ESP_OK) return;

    if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &__custom_wifi_handler,
                                                        NULL,
                                                        NULL)) != ESP_OK) return;


    wifi_config_t wifi_config;
    
    //wifi_config.ap.ssid = {};
    //wifi_config.ap.ssid_len = 0;
    wifi_config.ap.channel = chh;
    //wifi_config.ap.password = {};
    wifi_config.ap.max_connection = maxdevices;
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

    // [1..13]
    if (wifi_config.ap.channel > 13) wifi_config.ap.channel = 13;
    if (wifi_config.ap.channel < 1) wifi_config.ap.channel = 1;

    _wifi.ssid_cpy = sss.substr(0, std::size(wifi_config.ap.ssid) - 1);
    _wifi.pw_cpy = pw.substr(0, std::size(wifi_config.ap.password) - 1);

    sprintf((char*)wifi_config.ap.ssid, "%s", _wifi.ssid_cpy.c_str());
    wifi_config.ap.ssid_len = _wifi.ssid_cpy.size();

    if (pw.size()) {
        sprintf((char*)wifi_config.ap.password, "%s", _wifi.pw_cpy.c_str());
    }
    else {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_mode(WIFI_MODE_AP)) != ESP_OK) return;
    if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_config(WIFI_IF_AP, &wifi_config)) != ESP_OK) return;
    if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_start()) != ESP_OK) return;

    ESP_LOGI(TAGWIFI, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             wifi_config.ap.ssid, wifi_config.ap.password, wifi_config.ap.channel);

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_open_sockets = 7;
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAGWIFI, "Starting webserver on port: '%d'", config.server_port);
    if (httpd_start(&_wifi.webserv, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAGWIFI, "Registering URI handlers");
        //httpd_register_uri_handler(_wifi.webserv, &_webroot);
        if (autohttpfp) custom_wifi_add_handle("/", HTTP_GET, root_get_handler);
        httpd_register_err_handler(_wifi.webserv, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    
    start_dns_server();

    _wifi.qrcoder = "WIFI:T:WPA;S:" + _wifi.ssid_cpy + ";P:" + _wifi.pw_cpy + ";H:false;";
    _wifi.event_active = true;
    
    ESP_LOGI(TAGWIFI, "custom_wifi_start ended");
}

inline void custom_wifi_stop()
{
    auto& _wifi = get_singleton_wifi_info();

    if (!_wifi.event_active) return; 
    if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_stop()) != ESP_OK) return;
    if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_deinit()) != ESP_OK) return;
    if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_deinit()) != ESP_OK) return;
    //if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_loop_delete_default()) != ESP_OK) return;
    //if (ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_flash_deinit()) != ESP_OK) return;
    _wifi.connected_devices = 0;
    _wifi.event_active = false;
}

inline qrcodegen::QrCode custom_wifi_gen_QR()
{
    auto& _wifi = get_singleton_wifi_info();
    return qrcodegen::QrCode::encodeText(_wifi.qrcoder.c_str(), qrcodegen::QrCode::Ecc::ECC_LOW);
}

inline const std::string& custom_wifi_get_ssid()
{
    auto& _wifi = get_singleton_wifi_info();
    return _wifi.ssid_cpy;
}

inline const std::string& custom_wifi_get_password()
{
    auto& _wifi = get_singleton_wifi_info();
    return _wifi.pw_cpy;
}

inline const uint8_t& custom_wifi_get_count()
{
    auto& _wifi = get_singleton_wifi_info();
    return _wifi.connected_devices;
}

inline void __custom_wifi_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    auto& _wifi = get_singleton_wifi_info();
    switch(event_id) {
    case WIFI_EVENT_AP_STACONNECTED:
    {
        ++_wifi.connected_devices;
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAGWIFI, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
    }
        break;
    case WIFI_EVENT_AP_STADISCONNECTED:
    {
        --_wifi.connected_devices;
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAGWIFI, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
        break;
    default:
        ESP_LOGI(TAGWIFI, "station unhandled event (not error)");
        break;
    }
}