//#define _DEBUG
#include "Serial/packaging.h"

using namespace CS;

PackagedWired* wire;
constexpr int led_pin = 2;
bool must_check_again = true;

void setup()
{
    Serial.begin(115200);
    while(!Serial || Serial.available() == 0);
    
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH);
    
    Serial.printf("Starting MASTER\n");
    wire = new PackagedWired(config().set_master());    
    
    attachInterrupt(digitalPinToInterrupt(0), []{
        Serial.printf("User requested check. Set to check.\n");
        digitalWrite(led_pin, HIGH);    
        must_check_again = true;
    }, RISING);
    
    digitalWrite(led_pin, LOW);
}

void loop()
{
    Serial.printf("### Beginning new requests... ###\n");
    for(uint8_t p = 0; p < d2u(device_id::_MAX); ++p) {
        const device_id curr = static_cast<device_id>(p);
        const char* device_name = d2str(curr);
        
        //Serial.printf("Requesting %s... ", d2str(curr));
        
        const auto lst = wire->master_request_all(curr);
        
        for(const auto& i : lst) {
            Serial.printf("> %s: %s = ", device_name, i.get_path());
            switch(i.get_type()) {
            case Command::vtype::TD:
                Serial.print(i.get_val<double>());
                break;
            case Command::vtype::TF:
                Serial.print(i.get_val<float>());
                break;
            case Command::vtype::TI:
                Serial.print(i.get_val<int64_t>());
                break;
            case Command::vtype::TU:
                Serial.print(i.get_val<uint64_t>());
                break;
            default:
                Serial.print("Unknown");
                break;
            }
            Serial.println();
        }
    }
    Serial.printf("### Ended row of requests ###\n");
    
    delay(5000);
}