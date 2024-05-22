#include "defaults.h"
#include "sdcard.h"
#include "cpu_manager.h"

#include "Serial/protocol.h"

namespace SDcard {

    bool& ___sd_init_mem() {
        static bool _sd;
        return _sd;
    }

    bool sd_init()
    {    
        /*
        uint8_t ssPin = SS
        SPIClass & spi = SPI
        uint32_t frequency = 4000000
        const char * mountpoint = "/sd"
        uint8_t max_files = 5
        bool format_if_empty = false
        */
        if (!___sd_init_mem()) {
            ___sd_init_mem() = SD.begin(5 /*select pin*/, SPI, 4000000, "/sd", d2u(CS::device_id::_MAX) + 1/*max_files*/, false);
        }
        return ___sd_init_mem();
    }
    
    bool is_sd_init()
    {
        return ___sd_init_mem();
    }

    const char* sd_get_type()
    {
        switch(SD.cardType()) {
        case CARD_NONE:
            return "NON";
        case CARD_MMC:
            return "MMC";
        case CARD_SD:
            return "SD";
        case CARD_SDHC:
            return "SDHC";
        default:
            return "BROKE";
        }
    }

    bool f_remove(const char* src)
    {
        bool r = SD.remove(src);
        if (!r) { sd_init(); r = SD.remove(src); }
        return r;
    }

    bool f_rename(const char* src, const char* dst)
    {
        bool r = SD.rename(src, dst);
        if (!r) { sd_init(); r = SD.rename(src, dst); }
        return r;
    }

    File f_open(const char* src, const char* mode)
    {
        File f = SD.open(src, mode);
        if (!f) { sd_init(); f = SD.open(src, mode); }
        return f;
    }

    bool rmdir(const char* dir)
    {
        bool r = SD.rmdir(dir);
        if (!r) { sd_init(); r = SD.rmdir(dir); }
        return r;
    }

    bool mkdir(const char* dir)
    {
        bool r = SD.mkdir(dir);
        if (!r) { sd_init(); r = SD.mkdir(dir); }
        return r;
    }

    bool _list_dir(std::vector<dir_item>& v, const char* d, const size_t l)
    {
        Serial.printf("@ %s | %zu\n", d, l);
        File root = SD.open(d);

        if (!root) return false; // fail
        if (!root.isDirectory()) return true; // just not a directory

        for(File file = root.openNextFile(); file; file = root.openNextFile())
        {
            if (file.isDirectory()) {
                v.emplace_back(dir_item{file.name(), 0});
                if (l != 0) {
                    _list_dir(v, file.path(), l - 1);
                }
            }
            else {            
                v.emplace_back(dir_item{file.name(), file.size()});
            }
        }
        return true;
    }

    // deep = 0 -> list only current
    std::vector<dir_item> list_dir(const char* dir, const size_t how_deep)
    {
        std::vector<dir_item> v;
        if (!_list_dir(v, dir, how_deep)) {
            sd_init();
            _list_dir(v, dir, how_deep); // if fails, idc
        }
        return v;
    }

    size_t sd_size()
    {
        return SD.cardSize();
    }
    
    size_t sd_total()
    {
        return SD.totalBytes();
    }

    size_t sd_used()
    {
        return SD.usedBytes();
    }

}