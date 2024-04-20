#include <Arduino.h>
#include "Serial/package.h"

bool must_check_again = false;

void setup()
{
  Serial.begin(115200);
  while(!Serial);

  CustomSerial::begin_master();
  //CustomSerial::check_devices_online_if_not_checked();

  attachInterrupt(digitalPinToInterrupt(0), []{
    Serial.printf("User requested check. Set to check.\n");
    must_check_again = true;//CustomSerial::check_all_devices_online();
  }, RISING);
  
  // add button to check via attach using set_pin_to... later
}

void loop()
{
  if (!CustomSerial::is_any_device_connected() || must_check_again) {
    must_check_again = false;
    Serial.printf("Re-checking devices...\n");
    CustomSerial::check_all_devices_online();
    delay(5000);
    return;
  }
  
  delay(500);

  for (uint8_t id = 0; id < CustomSerial::get_devices_limit(); ++id) {
    if (!CustomSerial::is_device_connected(id)) {
      //Serial.printf("Skipped id %d (offline / unconfigured)...\n", id);
      continue;
    }

    Serial.printf("Requesting id %d...\n", id);
    CustomSerial::request(id);

    //Serial.printf("Wire has %d bytes to read.\n", Wire.available());

    CustomSerial::command_package cmds;
    CustomSerial::read(cmds);
    
    Serial.printf("Received %u infos.\n", static_cast<unsigned>(cmds.size()));

    for(size_t p = 0; p < cmds.size(); ++p) {
      const auto& cmd = cmds.idx(p);

      Serial.printf("Data came from %hu @ path %s: ",
          (uint16_t)cmd.get_device_id(), cmd.get_path());

      switch(cmd.get_val_type()) {
      case CustomSerial::command::vtype::TD:
        Serial.print(cmd.get_val().d);
        break;
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
        Serial.print("Unhandled.");
      }
      Serial.println();
      //delay(250);
    }
  }  
}