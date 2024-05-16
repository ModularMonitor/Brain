#pragma once

#include "Serial/packaging.h"
#include "Serial/flags.h"
#include "devices_data_bridge.h"
#include "cpu_manager.h"

constexpr decltype(millis()) loop_delay = 1000;

void loop_think(void* arg_useless)
{    
    PackagedWired* wire = new PackagedWired(config().set_master().set_led(13).set_sda(25).set_scl(26));
    ModuleMapping& mm = get_global_map();

    while(1) {
        AutoTiming autotime(1000);        
        
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
                    Serial.print(i.get_val<double>());
                    break;
                case Command::vtype::TF:
                    mm.push_data_for(curr, i.get_path(), i.get_val<float>());
                    Serial.print(i.get_val<float>());
                    break;
                case Command::vtype::TI:
                    mm.push_data_for(curr, i.get_path(), i.get_val<int64_t>());
                    Serial.print(i.get_val<int64_t>());
                    break;
                case Command::vtype::TU:
                    mm.push_data_for(curr, i.get_path(), i.get_val<uint64_t>());
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