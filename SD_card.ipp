#pragma once

#include "SD_card.h"
#include "LOG_ctl.h"

inline void MySDcard::async_sdcard_caller()
{
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

        while(m_last_type == SD_type::C_OFFLINE) {
            if (!SD.begin(cs, *spi, 4000000, "/sd", sd_max_files_open, false)) {
                SLEEP(3000);
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
        }
    };

    sd_card_checker();

    uint64_t last_time = 0;
    m_initialized = true;

    while(1) {
        // check & update type if necessary
        if (get_time_ms() - last_time > sd_check_sd_time_ms || last_time == 0) {
            //LOGI(e_LOG_TAG::TAG_SD, "Check SD existence time");

            auto fp = SD.open("/__check_card.txt", "w");
            const auto tp = SD.cardType();

            if (!fp || (tp != CARD_MMC && tp != CARD_SD && tp != CARD_SDHC)) {
                sd_card_checker();
            }

            last_time = get_time_ms();
        }

        if (m_tasks.empty()) {
            SLEEP(20);
            continue;
        }

        std::lock_guard<std::mutex> l(m_tasks_mtx);
        for(auto& i : m_tasks) {
            try {
                i();
            }
            catch(const std::exception& e) {
                Serial.printf("__SDCARD: EXCEPTION: %s", e.what());
            }
            catch(const std::exception& e) {
                Serial.printf("__SDCARD: EXCEPTION: Uncaught");
            }
        }
        m_tasks.clear();
        SLEEP(20);
        last_time = get_time_ms();
    }

    m_initialized = false;
}

inline void MySDcard::push_task(std::packaged_task<void(void)>&& moving)
{
    std::lock_guard<std::mutex> l(m_tasks_mtx);
    m_tasks.push_back(std::move(moving));
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
    push_task(std::move(task));
    fut.wait();

    return ret;
}

inline bool MySDcard::rename_file(const char* who, const char* to)
{
    bool ret = false;
    std::packaged_task<void(void)> task([&ret, &who, &to]() { ret = SD.rename(who, to); });

    auto fut = task.get_future();
    push_task(std::move(task));
    fut.wait();
    
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
    push_task(std::move(task));
    fut.wait();
    
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
    push_task(std::move(task));
    fut.wait();
    
    return ret;
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
    push_task(std::move(task));
    fut.wait();
    
    return ret;
}

inline bool MySDcard::remove_dir(const char* dir)
{
    bool ret = false;
    std::packaged_task<void(void)> task([&ret, &dir]() { ret = SD.rmdir(dir); });

    auto fut = task.get_future();
    push_task(std::move(task));
    fut.wait();
    
    return ret;
}

inline bool MySDcard::make_dir(const char* dir)
{
    bool ret = false;
    std::packaged_task<void(void)> task([&ret, &dir]() { ret = SD.mkdir(dir); });

    auto fut = task.get_future();
    push_task(std::move(task));
    fut.wait();
    
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
    push_task(std::move(task));
    fut.wait();
    
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
    push_task(std::move(task));
    fut.wait();
    
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