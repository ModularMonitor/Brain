/*
SD_card.h

# Depends on:
- defaults.h

# Description:
- This header contains the implementation of the synchronous SD card class "MySDcard", which is a singleton
and must be only referenced using the appropriate GET(MySDcard) as defaults.h defines it.
- This is an sync wrapper on the SD library from Arduino, with pins set from defaults.h, that allows any thread
to call SD functions.

*/

#pragma once

#include "defaults.h"

#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include <deque>
#include <string>
#include <mutex>
#include <memory>
#include <future>

enum class SD_type {C_OFFLINE, C_MMC, C_SD, C_SDHC};

struct file_info {
    std::string fp_name;
    uint32_t fp_size;
    bool is_file;
};

// use GET(MySDcard) to get its singleton ref!
// MySDcard is an async SD card manager that allows any thread to append tasks of write / read to the card. It also tests for the SD card periodically.
MAKE_SINGLETON_CLASS(MySDcard, {
    volatile bool m_initialized = false;

    std::deque<std::packaged_task<void(void)>> m_tasks;
    std::mutex m_tasks_mtx;
    SD_type m_last_type = SD_type::C_OFFLINE;

    void async_sdcard_caller();

    // assists on pushing to queue. False only if queue is too large already (super rare if you have SD card)
    bool push_task(std::packaged_task<void(void)>&&);
public:
    MySDcard();

    // Removes a file from SD card.
    bool remove_file(const char* who);

    // Get file size
    size_t get_file_size(const char* path);

    // Renames a file from SD card.
    bool rename_file(const char* who, const char* to);

    // Append to the end of a file, or create if not exists, data.
    size_t append_on(const char* path, const char* what, const size_t len);

    // Create or replace a file with data.
    size_t overwrite_on(const char* path, const char* what, const size_t len);

    // Read some data from file, with offset.
    size_t read_from(const char* path, char* buffer, const size_t len, const uint32_t seek = 0);

    // Abuse the sequential async queue to append to the end of a file, or create if not exists, data.
    void async_append_on(const char* path, const char* what, const size_t len);

    // Abuse the sequential async queue to create or replace a file with data.
    void async_overwrite_on(const char* path, const char* what, const size_t len);

    // Remove a directory.
    bool remove_dir(const char* dir);

    // Make a directory.
    bool make_dir(const char* dir);

    // Check if file exists (must not be a directory).
    bool file_exists(const char* who);

    // Check if directory exists (must be directory).
    bool dir_exists(const char* dir);

    // List files and dirs under this path:
    std::deque<file_info> ls(const char* dir);

    // Check SD card size.
    size_t sd_card_size() const;

    // Check max bytes that you can have in this SD card.
    size_t sd_max_bytes() const;

    // Check how much has been used in bytes so far.
    size_t sd_used_bytes() const;

    // Get used by max percentage, [0..1].
    float sd_used_perc() const;

    // Get SD card type currently. May change over time (inserting new one / removing one)
    SD_type sd_type() const;

    // Get current tasks size pending to be worked on.
    size_t tasks_size() const;

    // Get if it is online already
    bool is_running() const;

    // Get if SD card is functioning
    bool is_online() const;
});

//#include "SD_card.ipp"