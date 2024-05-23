#include "common.h"
#include "cpu_ctl.h" // self deploy
#include "display.h" // self deploy
#include "sdcard.h" // self deploy

#include "debug_tools.h"

void setup()
{
    Serial.begin(115200);
    Serial.printf("Started MASTER\n");
    
    //CPU::run_on_core_sync([](void* a){ SDcard::sd_init(); }, cpu_core_id_for_sd_card, nullptr);

    actcp(idc_loop_sometimes, 0, 1);
    actcp(idc_loop_sometimes, 1, 1);
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