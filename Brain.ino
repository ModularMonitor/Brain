//#define _DEBUG
#include "Serial/packaging.h"
#include "Serial/flags.h"

using namespace CS;

constexpr decltype(millis()) loop_delay = 1000;

PackagedWired* wire;
bool devices_online[static_cast<size_t>(device_id::_MAX)]{false};


void setup()
{
    Serial.begin(115200);
    while(!Serial);
    delay(2000);
    
    Serial.printf("Starting MASTER\n");
    wire = new PackagedWired(config().set_master().set_led(2));    
}

void loop()
{
    const auto bg = millis();
    
    for(uint8_t p = 0; p < d2u(device_id::_MAX); ++p) {
        const device_id curr = static_cast<device_id>(p);
        const char* device_name = d2str(curr);        
        
        FlagWrapper fw;
        auto lst = wire->master_smart_request_all(curr, fw);
        
        if (fw & device_flags::HAS_ISSUES) {
            Serial.printf("[!] %s has issues flag on! Please check device!\n", device_name);
            continue;
        }        
        
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
    
    const auto nd = millis();
    
    if (nd - bg < loop_delay) delay(loop_delay - (nd - bg));
}