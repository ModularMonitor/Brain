#pragma once

#include "Serial/packaging.h"
#include "Serial/flags.h"
#include "devices_data_bridge.h"
#include "cpu_manager.h"
#include "sdcard.h"

constexpr decltype(millis()) loop_delay = 1000;

void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const double val);
void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const float val);
void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const int64_t val);
void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const uint64_t val);

void loop_think(void* arg_useless)
{    
    PackagedWired* wire = new PackagedWired(config().set_master().set_led(13).set_sda(25).set_scl(26));
    ModuleMapping& mm = get_global_map();

    if (SDcard::mkdir("/i2c")) {
        for(uint8_t p = 0; p < d2u(device_id::_MAX); ++p) {
            char buf[80];
            snprintf(buf, 80, "/i2c/%s", d2str(static_cast<device_id>(p)));
            SDcard::mkdir(buf);
        }
    }

    // LATER: have a val in global map dynamic so time is configurable!
    TimingLoop tl[d2u(device_id::_MAX)]{1000,1000,1000,1000,1000,1000,1000};

    while(1) {
        AutoTiming autotime(loop_delay);        
        
        for(uint8_t p = 0; p < d2u(device_id::_MAX); ++p) {
            const device_id curr = static_cast<device_id>(p);
            const char* device_name = d2str(curr);        
            
            FlagWrapper fw;
            bool answered = false;
            auto lst = wire->master_smart_request_all(curr, fw, answered);
            bool has_issues = fw & device_flags::HAS_ISSUES;

            mm.set_online(curr, answered);
            mm.set_has_issues(curr, has_issues);
            
            if (!answered) continue;
            
            if (has_issues) {
                Serial.printf("[!] %s has issues flag on! Please check device!\n", device_name);
                continue;
            }        
            
            for(const auto& i : lst) {
                Serial.printf("> %s: %s = ", device_name, i.get_path());

                switch(i.get_type()) {
                case Command::vtype::TD:
                    mm.push_data_for(curr, i.get_path(), i.get_val<double>());
                    if (tl[p].is_time()) format_and_store_sd_card(get_time_ms(), curr, i.get_path(), i.get_val<double>());
                    Serial.print(i.get_val<double>());
                    break;
                case Command::vtype::TF:
                    mm.push_data_for(curr, i.get_path(), i.get_val<float>());
                    if (tl[p].is_time()) format_and_store_sd_card(get_time_ms(), curr, i.get_path(), i.get_val<float>());
                    Serial.print(i.get_val<float>());
                    break;
                case Command::vtype::TI:
                    mm.push_data_for(curr, i.get_path(), i.get_val<int64_t>());
                    if (tl[p].is_time()) format_and_store_sd_card(get_time_ms(), curr, i.get_path(), i.get_val<int64_t>());
                    Serial.print(i.get_val<int64_t>());
                    break;
                case Command::vtype::TU:
                    mm.push_data_for(curr, i.get_path(), i.get_val<uint64_t>());
                    if (tl[p].is_time()) format_and_store_sd_card(get_time_ms(), curr, i.get_path(), i.get_val<uint64_t>());
                    Serial.print(i.get_val<uint64_t>());
                    break;
                default:
                    Serial.print("Unknown");
                    break;
                }
                Serial.println();
            }
        }        
    }
}

// TODO: why isn't it working as that?

void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const double val)
{
    //char buf[80];
    //snprintf(buf, 80, "/i2c/%s", d2str(did));
    //File fp = SDcard::f_open("data.txt", "a");
    //fp.printf("%s = %.6lf\n", path, val);
}

void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const float val)
{
    //char buf[80];
    //snprintf(buf, 80, "/i2c/%s", d2str(did));
    //File fp = SDcard::f_open("data.txt", "a");
    //fp.printf("%s = %.4f\n", path, val);
}

void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const int64_t val)
{
    //char buf[80];
    //snprintf(buf, 80, "/i2c/%s", d2str(did));
    //File fp = SDcard::f_open("data.txt", "a");
    //fp.printf("%s = %lli\n", path, val);
}

void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const uint64_t val)
{
    //char buf[80];
    //snprintf(buf, 80, "/i2c/%s", d2str(did));
    //File fp = SDcard::f_open("data.txt", "a");
    //fp.printf("%s = %llu\n", path, val);
}
