#pragma once

#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include <vector>
#include <string>

namespace SDcard {

    static const char TAG[] = "SD";

    bool is_sd_init();
    const char* sd_get_type();

    bool f_remove(const char* src);
    bool f_rename(const char* src, const char* dst);
    File f_open(const char* src, const char* mode);
    bool rmdir(const char* dir);
    bool mkdir(const char* dir);

    struct dir_item {
        std::string name;
        size_t size = 0;

        dir_item(const std::string d, const size_t s) : name(d), size(s) {}

        bool is_dir() const { return size == 0; }
    };

    // deep = 0 -> list only current
    std::vector<dir_item> list_dir(const char* dir, const size_t how_deep = static_cast<size_t>(-1));

    size_t sd_size();
    size_t sd_total();
    size_t sd_used();

    float sd_used_perc();
}

#include "sdcard.ipp"