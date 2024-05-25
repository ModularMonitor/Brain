#include "common.h"
#include "serial_setup.h" // self deploy
#include "cpu_ctl.h" // self deploy
#include "sdcard.h" // self deploy
#include "display.h" // self deploy

#include "debug_tools.h"

//DP::Display* dsp = nullptr;

void setup()
{
    //dsp = new DP::Display();
    //CPU::run_on_core_sync([](void* a){ SDcard::sd_init(); }, cpu_core_id_for_sd_card, nullptr);


    attachInterrupt(digitalPinToInterrupt(0), []{ 
        if (CPU::get_time_ms() > 5000) 
            DP::get_singleton_of_ASYNC_DisplayTask().get_internal_variable().get_display()->toggle_debugging();
        }, RISING);

    //actcp(idc_loop_sometimes, 0, 1);
    //actcp(idc_loop_sometimes, 1, 1);
}

void loop()
{
    //Serial.printf("CPU: %.1f%% | %.1f%%:%.1f%%; RAM: %.2f%%\n", 
    //    CPU::get_cpu_usage() * 100.0f,
    //    CPU::get_cpu_usage(0) * 100.0f,
    //    CPU::get_cpu_usage(1) * 100.0f,
    //    CPU::get_ram_usage() * 100.0f
    //);
    //delay(1000);
    vTaskDelete(NULL);
}