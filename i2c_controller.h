#pragma once

#include "Serial/packaging.h"
#include "Serial/flags.h"
#include "data_displayed.h"
#include "cpu_ctl.h"
#include "sdcard.h"


namespace I2CC {

    static const char TAG[] = "I2C";

    constexpr decltype(millis()) loop_delay = 1000;

    //void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const double val);
    //void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const float val);
    //void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const int64_t val);
    //void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const uint64_t val);

    void make_paths();




    // TODO: why isn't it working as that?

//    void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const double val)
//    {
//        //char buf[80];
//        //snprintf(buf, 80, "/i2c/%s", d2str(did));
//        //File fp = SDcard::f_open("data.txt", "a");
//        //fp.printf("%s = %.6lf\n", path, val);
//    }
//
//    void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const float val)
//    {
//        //char buf[80];
//        //snprintf(buf, 80, "/i2c/%s", d2str(did));
//        //File fp = SDcard::f_open("data.txt", "a");
//        //fp.printf("%s = %.4f\n", path, val);
//    }
//
//    void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const int64_t val)
//    {
//        //char buf[80];
//        //snprintf(buf, 80, "/i2c/%s", d2str(did));
//        //File fp = SDcard::f_open("data.txt", "a");
//        //fp.printf("%s = %lli\n", path, val);
//    }
//
//    void format_and_store_sd_card(const uint64_t timestamp, const device_id did, const char* path, const uint64_t val)
//    {
//        //char buf[80];
//        //snprintf(buf, 80, "/i2c/%s", d2str(did));
//        //File fp = SDcard::f_open("data.txt", "a");
//        //fp.printf("%s = %llu\n", path, val);
//    }
}

#include "i2c_controller.ipp"