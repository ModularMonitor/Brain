#pragma once

#include "common.h"
#include "cpu_ctl.h"

#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#include "TinyGSM/src/TinyGSM.h"

namespace LTE {

    static const char TAG[] = "LTE";

    // set GSM PIN, if any
    #define GSM_PIN ""

    // Your GPRS credentials, if any
    static const char apn[]  = "java.claro.com.br";     //SET TO YOUR APN
    static const char gprsUser[] = "";
    static const char gprsPass[] = "";


    class SIM {
    public:
        enum class time_format{
            CLOCK_FULL,     // xx:xx:xx
            CLOCK_RESUMED,  // xx:xx
            DATE,           // xxxx/xx/xx
            BOTH_FULL       // xxxx/xx/xx xx:xx:xx
        };
        enum class time_type {
            HOUR_24, // 00-23 clock
            HOUR_12  // Appends AM/PM in the end
        };
    private:
        TinyGsm* m_modem = nullptr;

        CPU::AutoWait m_loc_upd{30000};
        CPU::AutoWait m_rssi_upd{15000};

        struct tm m_loc;
        int m_rssi = -1;

        void update_location();
        void update_rssi();
    public:
        void task();

        struct tm get_time() const;
        void get_time_str(char*, const size_t, const time_format = time_format::CLOCK_RESUMED, const time_type = time_type::HOUR_12) const;
    };

    RUN_ASYNC_ON_CORE_AUTO(SIM, LTEModule, task, cpu_core_id_for_4g_lte, 0);
}

#include "lte_at.ipp"