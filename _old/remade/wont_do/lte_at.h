#pragma once

#include "common.h"
#include "cpu_ctl.h"
#include "data_shared.h"

#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#include "TinyGSM/src/TinyGSM.h"

#include <memory>

namespace LTE {

    static const char TAG[] = "LTE";

    // set GSM PIN, if any
    #define GSM_PIN ""

    // Your GPRS credentials, if any
    static const char apn[]  = "java.claro.com.br";     //SET TO YOUR APN
    static const char gprsUser[] = "";
    static const char gprsPass[] = "";
   

    class SIM {
    private:
        std::unique_ptr<TinyGsm> m_modem;

        CPU::AutoWait m_loc_upd{600000}; // 10 min is more than enough
        CPU::AutoWait m_rssi_upd{15000}; // 15 seconds
        CPU::AutoWait m_refresh_tcp_upd{60000}; // 60 seconds

        struct tm m_loc;
        int m_rssi = -1;

        void update_location();
        void update_rssi();
        void update_test_send_tcp(); // temporary
    public:
        void task();
    };

    RUN_ASYNC_ON_CORE_AUTO(SIM, LTEModule, task, cpu_core_id_for_4g_lte, 0);
}

#include "lte_at.ipp"