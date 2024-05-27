#include "common.h"
#include "serial_setup.h" // self deploy
#include "cpu_ctl.h" // self deploy
#include "sdcard.h" // self deploy
#include "display.h" // self deploy
#include "i2c_controller.h" // self deploy

#include "debug_tools.h"







#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define SerialAT Serial1

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS


// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[]  = "java.claro.com.br";     //SET TO YOUR APN
const char gprsUser[] = "";
const char gprsPass[] = "";

#include <SPI.h>
#include <SD.h>

#define MODEM_PWRKEY    MODULE4G_K
#define MODEM_TX        MODULE4G_R
#define MODEM_RX        MODULE4G_T
#define MODEM_FLIGHT    MODULE4G_S


int counter, lastIndex, numberOfPieces = 24;
String pieces[24], input;


bool reply = false;


void modem_on() {

    /*
    MODEM_PWRKEY IO:4 The power-on signal of the modulator must be given to it,
    otherwise the modulator will not reply when the command is sent
    */
    pinMode(MODEM_PWRKEY, OUTPUT);
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(300); //Need delay
    digitalWrite(MODEM_PWRKEY, LOW);

    /*
    MODEM_FLIGHT IO:25 Modulator flight mode control,
    need to enable modulator, this pin must be set to high
    */
    pinMode(MODEM_FLIGHT, OUTPUT);
    digitalWrite(MODEM_FLIGHT, HIGH);

    
  int i = 40;
  Serial.print(F("\n# Startup #\n"));
  Serial.print(F("# Sending \"AT\" to Modem. Waiting for Response\n# "));
  while (i) {
    SerialAT.print("AT\r");

    // Show the User: we are doing something.
    Serial.print(F("."));
    delay(500);

    // Did the Modem send something?
    if (SerialAT.available()) {
      String r = SerialAT.readString();
      Serial.print("\n# Response:\n" + r);
      if ( r.indexOf("OK") >= 0 ) {
        reply = true;
        break;;
      } else {
        Serial.print(F("\n# "));
      }
    }

    // Did the User try to send something? Maybe he did not receive the first messages yet. Inform the User what is happening
    if (Serial.available() && !reply) {
      Serial.read();
      Serial.print(F("\n# Modem is not yet online."));
      Serial.print(F("\n# Sending \"AT\" to Modem. Waiting for Response\n# "));
    }

    // On the 5th try: Inform the User what is happening
    if(i == 35) {
      Serial.print(F("\n# Modem did not yet answer. Probably Power loss?\n"));
      Serial.print(F("# Sending \"AT\" to Modem. Waiting for Response\n# "));
    }
    delay(500);
    i--;
  }
  Serial.println(F("#\n"));
}





//DP::Display* dsp = nullptr;

void setup()
{
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    attachInterrupt(digitalPinToInterrupt(0), []{ 
        if (CPU::get_time_ms() > 5000) 
            DP::get_singleton_of_ASYNC_DisplayTask().get_internal_variable().set_debugging();
        }, RISING);
    modem_on();
    //dsp = new DP::Display();
    //CPU::run_on_core_sync([](void* a){ SDcard::sd_init(); }, cpu_core_id_for_sd_card, nullptr);


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
    //vTaskDelete(NULL);


    if (SerialAT.available()) {
        Serial.write(SerialAT.read());
    }
    if (Serial.available()) {
        int ch = Serial.read();
        if (ch != '\n' && ch != '\r') SerialAT.write(ch);
        else SerialAT.write('\r');
    }
    delay(1);
}