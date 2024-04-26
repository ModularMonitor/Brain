#include <Arduino.h>
#include "Serial/package.h"

bool must_check_again = true;
constexpr int led_pin = 2;

void setup()
{
    Serial.begin(115200);
    Serial.println("Waiting 5 sec to boot");
    delay(5000);
    while(!Serial);
    pinMode(led_pin, OUTPUT);
    
    digitalWrite(led_pin, HIGH);
    
    CustomSerial::set_logging(Serial);
    CustomSerial::print_info();
    CustomSerial::begin_master();
    
    digitalWrite(led_pin, LOW);
    
    attachInterrupt(digitalPinToInterrupt(0), []{
        Serial.printf("User requested check. Set to check.\n");
        digitalWrite(led_pin, HIGH);    
        must_check_again = true;
    }, RISING);
}

void loop()
{
    if (!CustomSerial::is_any_device_connected() || must_check_again) {
        digitalWrite(led_pin, HIGH);
        if (must_check_again) Serial.printf("Waiting for devices...\n");
        must_check_again = false;
        CustomSerial::check_all_devices_online();
        delay(4900);
        digitalWrite(led_pin, LOW);
        delay(100);
        return;
    }

  for (uint8_t id = 0; id < CustomSerial::get_devices_limit(); ++id) {
        if (!CustomSerial::is_device_connected(id)) continue;
    
        Serial.printf("%d ...", (int)id);
        delay(500);
        
        CustomSerial::request(id);
        CustomSerial::command_package cmds;
        CustomSerial::read(cmds);
        
        Serial.printf("! got %u infos:\n", static_cast<unsigned>(cmds.size()));
        
    
        for(size_t p = 0; p < cmds.size(); ++p) {
            delay(500);
            const auto& cmd = cmds.idx(p);
        
            Serial.printf("- %hu @ %s => ", (uint16_t)cmd.get_device_id(), cmd.get_path());
        
            switch(cmd.get_val_type()) {
            case CustomSerial::command::vtype::TF:
                Serial.print(cmd.get_val().f);
                break;
            case CustomSerial::command::vtype::TI:
                Serial.print(cmd.get_val().i);
                break;
            case CustomSerial::command::vtype::TU:
                Serial.print(cmd.get_val().u);
                break;
            default:
                Serial.print("Unhandled/error");
            }
            Serial.println();
        }
    }  
    delay(500);
}