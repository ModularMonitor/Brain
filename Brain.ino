#include "defaults.h"
#include "SD_card.h"

#include <Arduino.h>

#define TEST(TEST, ERR) if (!(TEST)) { Serial.println(ERR); SLEEP(5000); ESP.restart(); }

void test_sd(void* endd)
{
    bool& ended = *(bool*)endd;

    MySDcard& sd = GET(MySDcard); // wakes it up
    while (!sd.is_running()) SLEEP(20);

    while (sd.sd_type() == SD_type::C_OFFLINE) {
        Serial.println("Please insert a SD card.");
        SLEEP(5000);
    }

    char buf[64]{};
    if (sd.read_from("/test.txt", buf, 8) != 8) {
        Serial.println("Could not read 8 bytes from test.txt. Creating it then...");
        TEST(sd.overwrite_on("/test.txt", "00000000", 8) == 8, "Cannot write on card!");
        TEST(sd.read_from("/test.txt", buf, 8) == 8, "Cannot read back!");
    }
    Serial.print("Read file test.txt! Content: ");
    Serial.println(buf);

    int a = atoi(buf) + 1;
    snprintf(buf, 64, "%08d", a);

    TEST(sd.overwrite_on("/test.txt", buf, 8) == 8, "Cannot write on card!");
    Serial.print("Wrote on file test.txt! Content: ");
    Serial.println(buf);

    ended = true;

    vTaskDelete(NULL);
}

void setup()
{
    Serial.begin(115200);
    while(!Serial.available()) SLEEP(100);

    bool ended = false;

    Serial.println("Testing SD card core 0");
    create_task(test_sd, "CORE0", 0, 6144, (void*)&ended, 0);
    while(!ended) SLEEP(100);

    ended = false;

    Serial.println("Testing SD card core 1");
    create_task(test_sd, "CORE1", 0, 6144, (void*)&ended, 1);
    while(!ended) SLEEP(100);
}

void loop() { vTaskDelete(NULL); }