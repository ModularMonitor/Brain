#include <Arduino.h>
#include "Serial/package.h"


uint8_t g_c = 0;

void setup()
{
  Serial.begin(9600);
  while (!Serial);

	MMSerial::setup(MMSerial::device_id::MASTER);

  Serial.println("Begin!");
}



void loop()
{  
	auto v = MMSerial::read_data();
  if (!v) {
    switch(g_c = (g_c + 1) % 5) {
    case 0: Serial.println("No data in serial yet"); break;
    case 1: Serial.println("No data in serial yet."); break;
    case 2: Serial.println("No data in serial yet.."); break;
    case 3: Serial.println("No data in serial yet..."); break;
    default: Serial.println("No data in serial yet...."); break;
    }    
    delay(1000);
  }
  else {    
		switch (v->get_type()) {
		case MMSerial::data_type::T_F:
			Serial.print("Got float: ");
      Serial.print(v->get_path());
      Serial.print(" -> ");
      Serial.println(v->get_as_float());
			break;
		case MMSerial::data_type::T_D:
			Serial.print("Got double: ");
      Serial.print(v->get_path());
      Serial.print(" -> ");
      Serial.println(v->get_as_double());
			break;
		case MMSerial::data_type::T_I:
			Serial.print("Got integer: ");
      Serial.print(v->get_path());
      Serial.print(" -> ");
      Serial.println(v->get_as_int());
			break;
		case MMSerial::data_type::T_U:
			Serial.print("Got unsigned: ");
      Serial.print(v->get_path());
      Serial.print(" -> ");
      Serial.println(v->get_as_uint());
			break;
		default:
			Serial.println("Unknown error");
		}
    delay(100);
  }
}