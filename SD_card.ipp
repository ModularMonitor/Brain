#pragma once

#include "SD_card.h"
#include "LOG_ctl.h"

#include <chrono>

inline void MySDcard::async_sdcard_caller()
{
    LOGI_NOSD(e_LOG_TAG::TAG_SD, "SD card initializing...");

    constexpr const int& sck  = sd_card_pins[0];
    constexpr const int& miso = sd_card_pins[1];
    constexpr const int& mosi = sd_card_pins[2];
    constexpr const int& cs   = sd_card_pins[3];
 
    auto spi = std::unique_ptr<SPIClass>(new SPIClass(SPI_SDCARD));
    spi->begin(sck, miso, mosi, cs);
    pinMode(spi->pinSS(), OUTPUT);

    const auto sd_card_checker = [&] {
        m_last_type = SD_type::C_OFFLINE;
        SD.end();

        if (!SD.begin(cs, *spi, 4000000, "/sd", sd_max_files_open, false)) {
            if (!m_initialized) { // must initialize without SD card anyway
                LOGE_NOSD(e_LOG_TAG::TAG_SD, "__SDCARD: CORE LOAD WARN! SD card is not present, so many stuff MAY NOT be stable. Please consider adding one.");
                LOGE_NOSD(e_LOG_TAG::TAG_SD, "__SDCARD: CORE LOAD WARN! Starting application in 10 seconds anyway. Good luck.");
                SLEEP(10000);
            }
            else SLEEP(200);
        }
        else {
            switch(SD.cardType()) {
            case CARD_MMC:
                m_last_type = SD_type::C_MMC;
                break;
            case CARD_SD:
                m_last_type = SD_type::C_SD;
                break;
            case CARD_SDHC:
                m_last_type = SD_type::C_SDHC;
                break;
            default:
                m_last_type = SD_type::C_OFFLINE;
                break;
            }
        }

        return m_last_type != SD_type::C_OFFLINE;
    };

    LOGI_NOSD(e_LOG_TAG::TAG_SD, "Checking its existance...");

    sd_card_checker();

    uint64_t last_time = 0;
    m_initialized = true;

    LOGI_NOSD(e_LOG_TAG::TAG_SD, "Working.");

    while(1) {
        // check & update type if necessary
        if (get_time_ms() - last_time > sd_check_sd_time_ms || last_time == 0) {

            auto fp = SD.open("/__check_card.txt", "w");
            const auto tp = SD.cardType();

            if (!fp || (tp != CARD_MMC && tp != CARD_SD && tp != CARD_SDHC)) {
                LOGE_NOSD(e_LOG_TAG::TAG_SD, "SD card got offline?! Trying to reconnect it...");

                if (sd_card_checker()) {
                    last_time = get_time_ms(); // reset time if now good
                    LOGI_NOSD(e_LOG_TAG::TAG_SD, "SD card back online.");
                }
            }
            else last_time = get_time_ms(); // reset time if good
        }

        // tasks may run even if sd card is not present.

        if (m_tasks.empty()) {
            SLEEP(20);
            continue;
        }


        while(!m_tasks.empty()) {
            std::packaged_task<void(void)> i;

            {
                std::lock_guard<std::mutex> l(m_tasks_mtx);
                i = std::move(m_tasks.front());
                m_tasks.pop_front();
            }

            try {
                i();
            }
            catch(const std::exception& e) {
                LOGE_NOSD(e_LOG_TAG::TAG_SD, "__SDCARD: EXCEPTION: %s\n", e.what());
            }
            catch(...) {
                LOGE_NOSD(e_LOG_TAG::TAG_SD, "__SDCARD: EXCEPTION: Uncaught\n");
            }
        }

        SLEEP(20);
        last_time = get_time_ms();
    }

    m_initialized = false;
    vTaskDelete(NULL);
}

inline bool MySDcard::push_task(std::packaged_task<void(void)>&& moving)
{
    //if (!is_online()) {
    //    LOGW_NOSD(e_LOG_TAG::TAG_SD, "No task pushed for SD card this time. Skipped nearby task. SD card is not present.");
    //    return false;
    //}

    for (bool second_try = false;;) {
        {
            std::lock_guard<std::mutex> l(m_tasks_mtx);
            if (m_tasks.size() < sd_max_tasks_pending) {
                m_tasks.push_back(std::move(moving));
                return true;
            }
            else if (second_try) {
                if (!is_online())   LOGE_NOSD(e_LOG_TAG::TAG_SD, "SD card queue is too large! Skipping one task, unfortunately. SD card seems to be offline. Sad news!");
                else                LOGE_NOSD(e_LOG_TAG::TAG_SD, "SD card queue is too large! Skipping one task, unfortunately. Probably an issue with the SD card itself?");
                return false;
            }
        }

        const auto max_wait = get_time_ms() + sd_max_timeout_sd_card_full_of_tasks;
        LOGW_NOSD(e_LOG_TAG::TAG_SD, "SD card queue is too large! Waiting on timeout");
        while(get_time_ms() < max_wait && m_tasks.size() >= sd_max_tasks_pending) SLEEP(50);
        second_try = true;        
    }
}

inline MySDcard::MySDcard()
{
    async_class_method_pri(MySDcard, async_sdcard_caller, sd_thread_priority, cpu_core_id_for_sd_card);
    while(!m_initialized) SLEEP(100);
}


inline bool MySDcard::remove_file(const char* who)
{
    bool ret = false;
    std::packaged_task<void(void)> task([&ret, &who]() { ret = SD.remove(who); });

    auto fut = task.get_future();
    if (!push_task(std::move(task))) return ret;

    if (fut.wait_for(std::chrono::milliseconds(sd_max_timeout_wait_future)) != std::future_status::ready)
        LOGE_NOSD(e_LOG_TAG::TAG_SD, "Task response on SD card took too long to work! Timed out or deferred!");

    return ret;
}

inline size_t MySDcard::get_file_size(const char* path)
{
    size_t ret = 0;
    std::packaged_task<void(void)> task([&ret, &path]() { 
        File fp = SD.open(path, FILE_READ);
        if (!fp) return;

        ret = fp.size();
        fp.close();
    });

    auto fut = task.get_future();
    if (!push_task(std::move(task))) return ret;

    if (fut.wait_for(std::chrono::milliseconds(sd_max_timeout_wait_future)) != std::future_status::ready)
        LOGE_NOSD(e_LOG_TAG::TAG_SD, "Task response on SD card took too long to work! Timed out or deferred!");

    return ret;
}

inline bool MySDcard::rename_file(const char* who, const char* to)
{
    bool ret = false;
    std::packaged_task<void(void)> task([&ret, &who, &to]() { ret = SD.rename(who, to); });

    auto fut = task.get_future();
    if (!push_task(std::move(task))) return ret;

    if (fut.wait_for(std::chrono::milliseconds(sd_max_timeout_wait_future)) != std::future_status::ready)
        LOGE_NOSD(e_LOG_TAG::TAG_SD, "Task response on SD card took too long to work! Timed out or deferred!");
    
    return ret;
}

inline size_t MySDcard::append_on(const char* path, const char* what, const size_t len)
{
    size_t ret = 0;
    std::packaged_task<void(void)> task([&ret, &path, &what, &len]() { 
        File fp = SD.open(path, FILE_APPEND);
        if (!fp) return;

        ret = fp.write((uint8_t*)what, len);
        fp.close();
    });

    auto fut = task.get_future();
    if (!push_task(std::move(task))) return ret;
    
    if (fut.wait_for(std::chrono::milliseconds(sd_max_timeout_wait_future)) != std::future_status::ready)
        LOGE_NOSD(e_LOG_TAG::TAG_SD, "Task response on SD card took too long to work! Timed out or deferred!");
    
    return ret;
}

inline size_t MySDcard::overwrite_on(const char* path, const char* what, const size_t len)
{
    size_t ret = 0;
    std::packaged_task<void(void)> task([&ret, &path, &what, &len]() { 
        File fp = SD.open(path, FILE_WRITE);
        if (!fp) return;

        ret = fp.write((uint8_t*)what, len);
        fp.close();
    });

    auto fut = task.get_future();
    if (!push_task(std::move(task))) return ret;
    
    if (fut.wait_for(std::chrono::milliseconds(sd_max_timeout_wait_future)) != std::future_status::ready)
        LOGE_NOSD(e_LOG_TAG::TAG_SD, "Task response on SD card took too long to work! Timed out or deferred!");
    
    return ret;
}


inline void MySDcard::async_append_on(const char* path, const char* what, const size_t len)
{
    std::string path_cpy = path;
    std::unique_ptr<char[]> what_cpy = std::unique_ptr<char[]>(new char[len]);
    memcpy(what_cpy.get(), what, len);

    std::packaged_task<void(void)> task([path_cpy, what_mov = std::move(what_cpy), len]() { 
        File fp = SD.open(path_cpy.c_str(), FILE_APPEND);
        if (!fp) return;

        fp.write((uint8_t*)what_mov.get(), len);
        fp.close();
    });

    push_task(std::move(task));
}

inline void MySDcard::async_overwrite_on(const char* path, const char* what, const size_t len)
{
    std::string path_cpy = path;
    std::unique_ptr<char[]> what_cpy = std::unique_ptr<char[]>(new char[len]);
    memcpy(what_cpy.get(), what, len);

    std::packaged_task<void(void)> task([path_cpy, what_mov = std::move(what_cpy), len]() { 
        File fp = SD.open(path_cpy.c_str(), FILE_WRITE);
        if (!fp) return;

        fp.write((uint8_t*)what_mov.get(), len);
        fp.close();
    });

    push_task(std::move(task));
}

inline size_t MySDcard::read_from(const char* path, char* buffer, const size_t len, const uint32_t seek)
{
    size_t ret = false;
    std::packaged_task<void(void)> task([&ret, &path, &buffer, &len, &seek]() { 
        File fp = SD.open(path, FILE_READ);
        if (!fp) return;

        if (seek > 0) fp.seek(seek);

        ret = fp.readBytes((char*)buffer, len);
        fp.close();
    });

    auto fut = task.get_future();
    if (!push_task(std::move(task))) return ret;
    
    if (fut.wait_for(std::chrono::milliseconds(sd_max_timeout_wait_future)) != std::future_status::ready)
        LOGE_NOSD(e_LOG_TAG::TAG_SD, "Task response on SD card took too long to work! Timed out or deferred!");
    
    return ret;
}

inline bool MySDcard::remove_dir(const char* dir)
{
    bool ret = false;
    std::packaged_task<void(void)> task([&ret, &dir]() { ret = SD.rmdir(dir); });

    auto fut = task.get_future();
    if (!push_task(std::move(task))) return ret;
    
    if (fut.wait_for(std::chrono::milliseconds(sd_max_timeout_wait_future)) != std::future_status::ready)
        LOGE_NOSD(e_LOG_TAG::TAG_SD, "Task response on SD card took too long to work! Timed out or deferred!");
    
    return ret;
}

inline bool MySDcard::make_dir(const char* dir)
{
    bool ret = false;
    std::packaged_task<void(void)> task([&ret, &dir]() { ret = SD.mkdir(dir); });

    auto fut = task.get_future();
    if (!push_task(std::move(task))) return ret;
    
    if (fut.wait_for(std::chrono::milliseconds(sd_max_timeout_wait_future)) != std::future_status::ready)
        LOGE_NOSD(e_LOG_TAG::TAG_SD, "Task response on SD card took too long to work! Timed out or deferred!");
    
    return ret;
}

inline bool MySDcard::file_exists(const char* who)
{
    bool ret = false;
    std::packaged_task<void(void)> task([&ret, &who]() {         
        File fp = SD.open(who);
        ret = fp && !fp.isDirectory();
        if (fp) fp.close();
    });

    auto fut = task.get_future();
    if (!push_task(std::move(task))) return ret;
    
    if (fut.wait_for(std::chrono::milliseconds(sd_max_timeout_wait_future)) != std::future_status::ready)
        LOGE_NOSD(e_LOG_TAG::TAG_SD, "Task response on SD card took too long to work! Timed out or deferred!");
    
    return ret;
}

inline bool MySDcard::dir_exists(const char* dir)
{
    bool ret = false;
    std::packaged_task<void(void)> task([&ret, &dir]() {
        File fp = SD.open(dir);
        ret = fp && fp.isDirectory();
        if (fp) fp.close();
    });

    auto fut = task.get_future();
    if (!push_task(std::move(task))) return ret;
    
    if (fut.wait_for(std::chrono::milliseconds(sd_max_timeout_wait_future)) != std::future_status::ready)
        LOGE_NOSD(e_LOG_TAG::TAG_SD, "Task response on SD card took too long to work! Timed out or deferred!");
    
    return ret;
}

std::deque<file_info> MySDcard::ls(const char* dir)
{
    std::deque<file_info> ret;

    std::packaged_task<void(void)> task([&ret, &dir]() {
        File root = SD.open(dir);
        if (!root || !root.isDirectory()) return;
        
        for(File file = root.openNextFile(); file; file = root.openNextFile()) {
            ret.push_back(file_info{std::string(file.name()), file.size(), !file.isDirectory() });
        }
    });

    auto fut = task.get_future();
    if (!push_task(std::move(task))) return ret;
    
    if (fut.wait_for(std::chrono::milliseconds(sd_max_timeout_wait_future)) != std::future_status::ready)
        LOGE_NOSD(e_LOG_TAG::TAG_SD, "Task response on SD card took too long to work! Timed out or deferred!");
    
    return ret;
}

inline size_t MySDcard::sd_card_size() const
{
    return SD.cardSize();
}

inline size_t MySDcard::sd_max_bytes() const
{
    return SD.totalBytes();
}

inline size_t MySDcard::sd_used_bytes() const
{
    return SD.usedBytes();
}

inline float MySDcard::sd_used_perc() const
{
    return sd_used_bytes() * 1.0f / sd_max_bytes();
}

inline SD_type MySDcard::sd_type() const
{
    return m_last_type;
}

inline size_t MySDcard::tasks_size() const
{
    return m_tasks.size();
}

inline bool MySDcard::is_running() const
{
    return m_initialized;
}

inline bool MySDcard::is_online() const
{
    return m_last_type != SD_type::C_OFFLINE && m_initialized;
}