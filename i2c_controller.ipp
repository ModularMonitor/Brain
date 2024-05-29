#pragma once

#include "i2c_controller.h"

namespace I2CC {
    
    namespace _INTERNAL {
        MAKE_SINGLETON_CLASS_INIT_C(COMM_ASYNC_RUN,
        {
            actcpb(
                CS::PackagedWired* wire = new CS::PackagedWired(CS::config().set_master().set_sda(COMM_SDA).set_scl(COMM_SCL));
                //ModuleMapping& mm = get_global_map();
                ::STR::SharedData& shared = ::STR::get_singleton_of_SharedData();

                LOGI(TAG[0], "Starting package control in 2 seconds.");

                delay(2000);

                // LATER: have a val in global map dynamic so time is configurable!
                //::CPU::AutoWait tl[d2u(device_id::_MAX)]{1000,1000,1000,1000,1000,1000,1000};

                while(1) {
                    ::CPU::AutoWait autotime(loop_delay); // loop control
                
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
                        
                        if (has_issues) continue;
                        
                        for(const auto& i : lst) {

                            switch(i.get_type()) {
                            case CS::Command::vtype::TD:
                                device_itself.update_data(i.get_path(), i.get_val<double>());
                                //LOGI(TAG[0], "+ %s: %s => %.8lf", device_name, i.get_path(), i.get_val<double>());
                                //if (tl[p].is_time()) format_and_store_sd_card(get_time_ms(), curr, i.get_path(), i.get_val<double>());
                                //Serial.print(i.get_val<double>());
                                break;
                            case CS::Command::vtype::TF:
                                device_itself.update_data(i.get_path(), i.get_val<float>());
                                //LOGI(TAG[0], "+ %s: %s => %.6f", device_name, i.get_path(), i.get_val<float>());
                                //if (tl[p].is_time()) format_and_store_sd_card(get_time_ms(), curr, i.get_path(), i.get_val<float>());
                                //Serial.print(i.get_val<float>());
                                break;
                            case CS::Command::vtype::TI:
                                device_itself.update_data(i.get_path(), i.get_val<int64_t>());
                                //LOGI(TAG[0], "+ %s: %s => %" PRIi64, device_name, i.get_path(), i.get_val<int64_t>());
                                //if (tl[p].is_time()) format_and_store_sd_card(get_time_ms(), curr, i.get_path(), i.get_val<int64_t>());
                                //Serial.print(i.get_val<int64_t>());
                                break;
                            case CS::Command::vtype::TU:
                                device_itself.update_data(i.get_path(), i.get_val<uint64_t>());
                                //LOGI(TAG[0], "+ %s: %s => %" PRIu64, device_name, i.get_path(), i.get_val<uint64_t>());
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

        MAKE_SINGLETON_CLASS_INIT_C(STORE_ASYNC_RUN,
        {
            actcpb(

                {
                    LOGI(TAG[1], "SD/InfluxDB thread is in development and will suspend forever for now (actually, it will free resources and leave).");
                    vTaskDelete(NULL);
                    while(1); // if somehow
                }

                ::STR::SharedData& shared = ::STR::get_singleton_of_SharedData();
                bool made_paths = false;

                const auto check_sdcard = [&]{                                        
                    LOGI(TAG[1], "Working on SD card paths...");
                    made_paths = make_paths(TAG[1]);

                    if (!made_paths) {
                        LOGW(TAG[1], "Could not make paths. SD card broken? Trying again soon.");
                        return false;
                    }
                    else {
                        LOGI(TAG[1], "Made paths or paths already exist. All good!");
                        return true;
                    }
                };
                
                LOGI(TAG[1], "Waiting 2 seconds to begin work.");
                check_sdcard();

                delay(2000);

                ::CPU::AutoWait min_delay{5000}; // <<<<<<<<<<<<<<<<<<< CHANGE LATER TO 1 SEC
                ::CPU::AutoWait sdcard_checker{120000};               
                std::unique_ptr<::CPU::AutoWait> devices_wait_sd[CS::d2u(CS::device_id::_MAX)]; // sd card
                std::unique_ptr<::CPU::AutoWait> devices_wait_idb[CS::d2u(CS::device_id::_MAX)]; // influx db send

                const auto update_device_wait_timing = [&]{                    
                    for(uint8_t p = 0; p < CS::d2u(CS::device_id::_MAX); ++p) {
                        const ::STR::StoredDataEachDevice& device_itself = shared(static_cast<CS::device_id>(p));

                        const uint32_t time_sd = device_itself.get_store_sd_card();
                        const uint32_t time_idb = device_itself.get_send_influx_db();

                        if (time_sd != 0) devices_wait_sd[p] = std::unique_ptr<::CPU::AutoWait>(new ::CPU::AutoWait{static_cast<uint64_t>(time_sd) * 1000});
                        else devices_wait_sd[p].reset();
                        if (time_idb != 0) devices_wait_idb[p] = std::unique_ptr<::CPU::AutoWait>(new ::CPU::AutoWait{static_cast<uint64_t>(time_idb) * 1000});
                        else devices_wait_idb[p].reset();
                    }
                };

                update_device_wait_timing();

                while(1) {
                    if (!SDcard::is_sd_init()) {
                        LOGW(TAG[1], "SD card is offline. Service suspended.");
                        while(!SDcard::is_sd_init()) delay(500);
                        LOGI(TAG[1], "SD card back, service restarting...");
                    }

                    if (!made_paths || sdcard_checker.is_time()) {
                        if (!check_sdcard()) {
                            delay(5000);
                            continue;
                        }
                    }

                    if (!min_delay.is_time()) {
                        delay(50);
                        continue;
                    }
                
                    for(uint8_t p = 0; p < CS::d2u(CS::device_id::_MAX); ++p) {
                        const auto curr = static_cast<CS::device_id>(p);
                        const char* device_name = CS::d2str(curr);        
                        const ::STR::StoredDataEachDevice& device_itself = shared(curr);
                        
                        if (!device_itself.get_is_online()) continue;

                        char buf[96 + CS::max_path_len];
                        char time_now[96];
                        shared.get_sim_data().get_time(time_now, 96);

                        if (device_itself.get_has_issues()) {
                            snprintf(buf, 96 + CS::max_path_len, "/i2c/%s/had_issues.txt", CS::d2str(static_cast<CS::device_id>(p)));
                            File fp = SDcard::f_open(buf, "a");
                            if (!fp) {
                                LOGE(TAG[1], "Could not open %s! Failed to manage device %s.", buf, device_name);
                                continue;
                            }

                            fp.printf("%s: HAD ISSUES!\n", time_now);
                            fp.close();
                            continue;
                        }
                        
                        for(const auto& i : device_itself) { // i-> == RepresentedData

                            if (devices_wait_sd[p] && devices_wait_sd[p]->is_time()) {
                                snprintf(buf, 96 + CS::max_path_len, "/i2c/%s%s", CS::d2str(static_cast<CS::device_id>(p)), i->get_path());
                                
                                File fp = SDcard::f_open(buf, "a");
                                if (!fp) {
                                    LOGE(TAG[1], "Could not open %s! Failed to manage device %s.", buf, device_name);
                                    continue;
                                }

                                fp.printf("%s: %s\n", time_now, i->get_in_time(0)); // for now consider continuous data coming
                                fp.close();
                            }
                            if (devices_wait_idb[p] && devices_wait_idb[p]->is_time()) { 





                                // TODO TBD InfluxDB is not a thing yet







                            }
                        }
                    }
                }            
            , cpu_core_id_for_ctl, 10);
        });
    }


    inline bool make_paths(const char* who) {
        if (SDcard::dir_exists("/i2c") || SDcard::mkdir("/i2c")) {
            LOGI(who, "Preparing SD card ground for devices...");
            for(uint8_t p = 0; p < CS::d2u(CS::device_id::_MAX); ++p) {
                char buf[80];
                snprintf(buf, 80, "/i2c/%s", CS::d2str(static_cast<CS::device_id>(p)));
                if (!SDcard::dir_exists(buf)) ::SDcard::mkdir(buf);
            }
            LOGI(who, "Paths are ready!");
            return true;
        }
        else {
            LOGW(who, "SD card not present. Not saving data.");
            return false;
        }
    }
}