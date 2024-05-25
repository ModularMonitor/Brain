#pragma once

#include "i2c_controller.h"

namespace I2CC {
    
    namespace _INTERNAL {
        MAKE_SINGLETON_CLASS_INIT_C(ASYNC_RUN,
        {
            actcpb(
                CS::PackagedWired* wire = new CS::PackagedWired(CS::config().set_master().set_led(COMM_LED).set_sda(COMM_SDA).set_scl(COMM_SCL));
                //ModuleMapping& mm = get_global_map();
                ::STR::SharedData& shared = ::STR::get_singleton_of_SharedData();

                LOGI(TAG, "Making first attempt to create/check paths...");
                make_paths();
                LOGI(TAG, "Started package control.");

                // LATER: have a val in global map dynamic so time is configurable!
                //::CPU::AutoWait tl[d2u(device_id::_MAX)]{1000,1000,1000,1000,1000,1000,1000};

                while(1) {
                    ::CPU::AutoWait autotime(loop_delay);        
                
                    for(uint8_t p = 0; p < CS::d2u(CS::device_id::_MAX); ++p) {
                        const auto curr = static_cast<CS::device_id>(p);
                        const char* device_name = CS::d2str(curr);        
                        ::STR::StoredDataEachDevice& device_itself = shared(curr);
                        
                        CS::FlagWrapper fw;
                        bool answered = false;
                        auto lst = wire->master_smart_request_all(curr, fw, answered);
                        bool has_issues = fw & CS::device_flags::HAS_ISSUES;

                        device_itself.set_is_online(answered);
                        device_itself.set_has_issues(has_issues);
                        
                        if (!answered) continue;
                        
                        if (has_issues) {
                            //Serial.printf("[!] %s has issues flag on! Please check device!\n", device_name);
                            continue;
                        }        
                        
                        for(const auto& i : lst) {
                            //Serial.printf("> %s: %s = ", device_name, i.get_path());

                            switch(i.get_type()) {
                            case CS::Command::vtype::TD:
                                device_itself.update_data(i.get_path(), i.get_val<double>());
                                LOGI(TAG, "+ %s: %s => %.8lf", device_name, i.get_path(), i.get_val<double>());
                                //if (tl[p].is_time()) format_and_store_sd_card(get_time_ms(), curr, i.get_path(), i.get_val<double>());
                                //Serial.print(i.get_val<double>());
                                break;
                            case CS::Command::vtype::TF:
                                device_itself.update_data(i.get_path(), i.get_val<float>());
                                LOGI(TAG, "+ %s: %s => %.6f", device_name, i.get_path(), i.get_val<float>());
                                //if (tl[p].is_time()) format_and_store_sd_card(get_time_ms(), curr, i.get_path(), i.get_val<float>());
                                //Serial.print(i.get_val<float>());
                                break;
                            case CS::Command::vtype::TI:
                                device_itself.update_data(i.get_path(), i.get_val<int64_t>());
                                LOGI(TAG, "+ %s: %s => %" PRIi64, device_name, i.get_path(), i.get_val<int64_t>());
                                //if (tl[p].is_time()) format_and_store_sd_card(get_time_ms(), curr, i.get_path(), i.get_val<int64_t>());
                                //Serial.print(i.get_val<int64_t>());
                                break;
                            case CS::Command::vtype::TU:
                                device_itself.update_data(i.get_path(), i.get_val<uint64_t>());
                                LOGI(TAG, "+ %s: %s => %" PRIu64, device_name, i.get_path(), i.get_val<uint64_t>());
                                //if (tl[p].is_time()) format_and_store_sd_card(get_time_ms(), curr, i.get_path(), i.get_val<uint64_t>());
                                //Serial.print(i.get_val<uint64_t>());
                                break;
                            default:
                                //Serial.print("Unknown");
                                break;
                            }
                            //Serial.println();
                        }
                    }
                }            
            , cpu_core_id_for_ctl, 2);
        });
    }


    inline void make_paths() {
        if (SDcard::dir_exists("/i2c") || SDcard::mkdir("/i2c")) {
            LOGI(TAG, "Preparing SD card ground for devices...");
            for(uint8_t p = 0; p < CS::d2u(CS::device_id::_MAX); ++p) {
                char buf[80];
                snprintf(buf, 80, "/i2c/%s", CS::d2str(static_cast<CS::device_id>(p)));
                if (!SDcard::dir_exists(buf)) ::SDcard::mkdir(buf);
            }
            LOGI(TAG, "Paths are ready!");
        }
        else {
            LOGW(TAG, "SD card not present. Not saving data.");            
        }
    }
}