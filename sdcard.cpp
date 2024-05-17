#include "sdcard.h"

namespace SDcard {

    bool& ___sd_init_mem() {
        static bool _sd = false;
        return _sd;
    }

    bool sd_init()
    {    
        if (!___sd_init_mem())  ___sd_init_mem() |= SD.begin();
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
            return "UNK";
        }
    }

    bool f_remove(const char* src)
    {
        return SD.remove(src);
    }

    bool f_rename(const char* src, const char* dst)
    {
        return SD.rename(src, dst);
    }

    File f_open(const char* src, const char* mode)
    {
        return SD.open(src, mode);
    }

    bool rmdir(const char* dir)
    {
        return SD.rmdir(dir);
    }

    bool mkdir(const char* dir)
    {
        return SD.mkdir(dir);
    }

    void _list_dir(std::vector<dir_item>& v, const char* d, const size_t l)
    {
        Serial.printf("@ %s | %zu\n", d, l);
        File root = SD.open(d);
        if (!root || !root.isDirectory()) return;

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
    }

    // deep = 0 -> list only current
    std::vector<dir_item> list_dir(const char* dir, const size_t how_deep)
    {
        std::vector<dir_item> v;
        _list_dir(v, dir, how_deep);
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