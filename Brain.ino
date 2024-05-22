#include "common.h"
#include "cpu_ctl.h"
#include "display.h"
#include "sdcard.h"


//class ASYNC_DisplayTask {
//public:
//    ASYNC_DisplayTask() {
//        {
//            CPU::create_task([](void*){ DP::Display obj; while(1) { obj.task(); yield(); }; vTaskDelete(__null); }, "ASYNCAUTO", 2, cpu_stack_default, nullptr, cpu_core_id_for_display);
//        };
//    }
//};
//ASYNC_DisplayTask& get_singleton_of_ASYNC_DisplayTask(){ static ASYNC_DisplayTask obj; return obj; }
//namespace ______dummy_namespace_for_ASYNC_DisplayTask{ const auto& ______dummy_ref_ASYNC_DisplayTask = get_singleton_of_ASYNC_DisplayTask(); }

void setup()
{
    Serial.begin(115200);
    Serial.printf("Started MASTER\n");
    
    actcp(idc_loop_sometimes, 0, 1);
    actcp(idc_loop_sometimes, 1, 1);
}

void loop()
{
    Serial.printf("CPU: %.1f%% | %.1f%%:%.1f%%; RAM: %.2f%%\n", 
        CPU::get_cpu_usage() * 100.0f,
        CPU::get_cpu_usage(0) * 100.0f,
        CPU::get_cpu_usage(1) * 100.0f,
        CPU::get_ram_usage() * 100.0f
    );
    delay(1000);
    //vTaskDelete(NULL);
}

void idc_loop_sometimes(void*)
{
    int random_store = 0;

    while(1)
    {
        const int rnd = 7000 + (CPU::get_random() % 3000);
        Serial.printf("### %i SLEEEEEEEP %i msec\n", (int)CPU::get_core_id(), rnd);
        delay(rnd);

        CPU::AutoWait aw(6000);
        Serial.printf("### %i RUUUUUUUUN\n", (int)CPU::get_core_id());

        while(!aw.is_time()) {
            char ch = 0;
            for(int a = 0; a < 10; ++a) {
                ch += CPU::get_random();
                if (ch % 7 > 3) random_store += 6345 * (int)ch;
                else random_store -= 46532 + CPU::get_random();
            }
            yield();
        }
    }
}