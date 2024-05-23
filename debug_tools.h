#pragma once

#include "common.h"
#include "cpu_ctl.h" // self deploy

inline void idc_loop_sometimes(void*)
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
            for(int a = 0; a < 5; ++a) {
                ch += CPU::get_random();
                if (ch % 7 > 3) random_store += 6345 * (int)ch;
                else random_store -= 46532 + CPU::get_random();
            }
            yield();
        }
    }
}