#pragma once

#include "sdcard.h"
#include "common.h"
#include "cpu_ctl.h"

#include "Serial/protocol.h"

namespace SDcard {

    namespace _INTERNAL {       


        //inline bool& ___sd_init_mem() {
        //    static bool _sd;
        //    return _sd;
        //}
        //
        //inline bool sd_init()
        //{
        //    if (CPU::get_core_id() != def_alt_core_id) return false;
        //
        //    static SPIClass* vspi = nullptr;
        //
        //    if (!vspi) {
        //        vspi = new SPIClass(SPI_SDCARD);
        //        vspi->begin(SDCARD_SCK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);
        //        pinMode(vspi->pinSS(), OUTPUT);
        //    }
        //
        //
        //    /*
        //    uint8_t ssPin = SS
        //    SPIClass & spi = SPI
        //    uint32_t frequency = 4000000
        //    const char * mountpoint = "/sd"
        //    uint8_t max_files = 5
        //    bool format_if_empty = false
        //    */
        //    if (!___sd_init_mem()) {
        //        ___sd_init_mem() = SD.begin(SDCARD_CS /*select pin*/, *vspi, 4000000, "/sd", d2u(CS::device_id::_MAX) + 1/*max_files*/, false);            
        //    }
        //    return ___sd_init_mem();
        //}
        //
        //
        //
        //inline bool is_sd_init()
        //{
        //    return ___sd_init_mem();
        //}

        MAKE_SINGLETON_CLASS_INIT_C(INITIALIZER,
        {
            actcpb(    
                SPIClass* vspi = new SPIClass(SPI_SDCARD);
                vspi->begin(SDCARD_SCK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);
                pinMode(vspi->pinSS(), OUTPUT);

                while(1) {
                    SD.exists("test.txt"); // dummy call
                    const auto noww = SD.cardType();
                    
                    if (noww != CARD_MMC && noww != CARD_SD && noww != CARD_SDHC) {
                        Serial.printf("[SD] SDcard removed / not loaded!\n");
                        SD.end();
                        while(1) {
                            const bool got = SD.begin(SDCARD_CS /*select pin*/, *vspi, 4000000, "/sd", d2u(CS::device_id::_MAX) + 1/*max_files*/, false);
                            if (!got) delay(3000);
                            else break;
                        }

                        Serial.printf("[SD] SDcard inserted / loaded!\n");
                    }
                    else {
                        delay(5000);
                    }
                }
            , cpu_core_id_for_sd_card, 10);
        });

    }

    inline const char* sd_get_type()
    {
        switch(SD.cardType()) {
        case CARD_NONE:
            return "OFF";
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

    inline bool f_remove(const char* src)
    {
        if (CPU::get_core_id() != cpu_core_id_for_sd_card) { Serial.println("[SD] GOT CALL FROM WRONG CORE, ABORTING F_REMOVE."); return false; }
        return SD.remove(src);
    }

    inline bool f_rename(const char* src, const char* dst)
    {
        if (CPU::get_core_id() != cpu_core_id_for_sd_card) { Serial.println("[SD] GOT CALL FROM WRONG CORE, ABORTING F_RENAME."); return false; }
        return SD.rename(src, dst);
    }

    inline File f_open(const char* src, const char* mode)
    {
        if (CPU::get_core_id() != cpu_core_id_for_sd_card) { Serial.println("[SD] GOT CALL FROM WRONG CORE, ABORTING F_OPEN."); return {}; }
        return SD.open(src, mode);
    }

    inline bool rmdir(const char* dir)
    {
        if (CPU::get_core_id() != cpu_core_id_for_sd_card) { Serial.println("[SD] GOT CALL FROM WRONG CORE, ABORTING RMDIR."); return false; }
        return SD.rmdir(dir);
    }

    inline bool mkdir(const char* dir)
    {
        if (CPU::get_core_id() != cpu_core_id_for_sd_card) { Serial.println("[SD] GOT CALL FROM WRONG CORE, ABORTING MKDIR."); return false; }
        return SD.mkdir(dir);
    }

    inline bool _list_dir(std::vector<dir_item>& v, const char* d, const size_t l)
    {
        if (CPU::get_core_id() != cpu_core_id_for_sd_card) { Serial.println("[SD] GOT CALL FROM WRONG CORE, ABORTING _LIST_DIR."); return false; }

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
    inline std::vector<dir_item> list_dir(const char* dir, const size_t how_deep)
    {
        if (CPU::get_core_id() != def_alt_core_id) { Serial.println("[SD] GOT CALL FROM WRONG CORE, ABORTING LIST_DIR."); return {}; }

        std::vector<dir_item> v;
        if (!_list_dir(v, dir, how_deep)) {
            sd_init();
            _list_dir(v, dir, how_deep); // if fails, idc
        }
        return v;
    }

    inline size_t sd_size()
    {
        return SD.cardSize();
    }
    
    inline size_t sd_total()
    {
        return SD.totalBytes();
    }

    inline size_t sd_used()
    {
        return SD.usedBytes();
    }

    inline float sd_used_perc()
    {
        return sd_used() * 1.0f / sd_size();
    }

}