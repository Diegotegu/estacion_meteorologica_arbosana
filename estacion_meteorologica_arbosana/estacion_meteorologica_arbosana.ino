#include <SigFox.h>
#include <Wire.h>
#include <SPI.h>
#include <ArduinoLowPower.h>
#include "conversions.h"

//mensaje principal
typedef struct __attribute__ ((packed)) sigfox_message {
  uint8_t status;
  uint16_t direccion;
  uint16_t velocidad;
  uint16_t temperatura;
  uint16_t humedad;
  uint16_t presion;
  uint8_t lastMessageStatus;
} SigfoxMessage;

//mensaje secundario
typedef struct __attribute__ ((packed)) sigfox_message_2 {
  uint8_t status;
  uint16_t lluvia;
  uint16_t intensidad_lluvia;
  uint16_t granizo;
  uint16_t intensidad_granizo;
  uint16_t  placa_temp;
  uint8_t lastMessageStatus;
} SigfoxMessage_2;


SigfoxMessage msg;
SigfoxMessage_2 msg_2;

String respuesta;
float direccion;
float velocidad, temperatura, presion, humedad, lluvia, lluvia_intensidad, granizo, granizo_intensidad;
char buff;
int comas[11];
String secciones[10];
bool debugger = true;
int oneshot = true;
#define freq_msg 15 //Frequency (in minutes for message)

void dummy() {
  volatile int aaa = 0;
  reboot();
}


void setup() {
  LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, dummy, CHANGE); //For LowPower Bug
  Serial1.begin(19200);
  if (debugger) {
    Serial.begin(19200);
    while (!Serial);
  }

  delay(500);
  Serial.println("iniciando sigfox");
  if (!SigFox.begin()) {
    // Something is really wrong, try rebooting
    // Reboot is useful if we are powering the board using an unreliable power source
    // (eg. solar panels or other energy harvesting methods)
    reboot();
  }
  //
  //  //Send module to standby until we need to send a message
  SigFox.end();

  if (oneshot == true) {
    // Enable debug prints and LED indication if we are testing
    SigFox.debug();
  }


}

void loop() {
  respuesta = "";
  buff = 0;
  Serial.println("enviando petición");
  Serial1.println("0R0");
  delay(25);

  while (!Serial1.available());
  while (Serial1.available()) {
    Serial.print(buff);
    buff = Serial1.read();
    respuesta += buff;
    delay(100);
  }
  Serial.println();

  int a = 0;
  for (int i = 0; i < respuesta.length(); i++) {
    if (respuesta.charAt(i) == ',') {
      comas[a] = i;
      a++;
    }
  }
  for (int i = 1; i <= a; i++) {
    secciones[i - 1] = respuesta.substring(comas[i - 1] + 4, comas[i] - 1);
  }
  
  Serial.println("respuesta recibida, guardando en variables");
  direccion = secciones[0].toFloat();
  velocidad = secciones[1].toFloat();
  temperatura = secciones[2].toFloat();
  humedad = secciones[3].toFloat();
  presion = secciones[4].toFloat();
  lluvia = secciones[5].toFloat(); //lluvia acumulada e intensidad de lluvia
  lluvia_intensidad = secciones[6].toFloat();
  granizo = secciones[7].toFloat();//granizo acumulado e intensidad de granizo
  granizo_intensidad = secciones[8].toFloat();


  msg.direccion = convertoFloatToUInt16(direccion, 360);
  msg.velocidad = convertoFloatToUInt16(velocidad, 50);
  msg.temperatura = convertoFloatToUInt16(temperatura, 60, -20);
  msg.humedad = convertoFloatToUInt16(humedad, 0, 110);
  msg.presion = convertoFloatToUInt16(presion, 1500, 500);
  msg_2.lluvia = convertoFloatToUInt16(lluvia, 1000);
  msg_2.intensidad_lluvia = convertoFloatToUInt16(lluvia_intensidad, 100);
  msg_2.granizo = convertoFloatToUInt16(granizo, 1000);
  msg_2.intensidad_granizo = convertoFloatToUInt16(granizo_intensidad, 100);

  /********************************************************************************************************************
    msg.status = 1;
    msg_2.status = 255;
    Serial.println("iniciando el envío");
     //Start the module
      SigFox.begin();
      // Wait at least 30ms after first configuration (100ms before)
      delay(100);
      SigFox.status();
      delay(1);
      SigFox.beginPacket();
      SigFox.write((uint8_t*)&msg, 12);
      msg.lastMessageStatus = SigFox.endPacket(true);
      SigFox.end();

      Serial.println("primer envio realizado, segundo envio");

      if (lluvia || granizo) {
        Serial.println("hay lluvia o granizo, enviando datos");
        SigFox.begin();
        // Wait at least 30ms after first configuration (100ms before)
        delay(100);
        float  temp_module = SigFox.internalTemperature();
        msg_2.placa_temp = convertoFloatToUInt16(temp_module, 60, -60);
        SigFox.status();
        delay(1);
        SigFox.beginPacket();
        SigFox.write((uint8_t*)&msg_2, 12);
        msg_2.lastMessageStatus = SigFox.endPacket(true);
        SigFox.end();


    }

    /*******************************************************************************************************************
    if (debugger) {
    muestratodo();
    }
  */
//  LowPower.sleep(freq_msg * 60 * 1000);
  //delay(5000);
}

void muestratodo() {
  Serial.println("----------------------------------------------------------------------------------------------------");
  Serial.print("estoy recibiendo: " + respuesta);
    for (int i = 0; i <= 8; i++) {
      Serial.println(secciones[i]);
    }
  Serial.print("posicion de las comas:");
  for (int i = 0; i <= 11; i++) {
    Serial.print(String(comas[i]) + ", ");
  }
  Serial.println();

  Serial.println("----------------------------------------------------------------------------------------------------");
  Serial.println("direccion del viento: " + String(direccion));
  Serial.println("velocidad del viento: " + String(velocidad));
  Serial.println("Temperatura ambiente: " + String(temperatura));
  Serial.println("humedad relativa: " + String(humedad));
  Serial.println("Presión atmosférica: " + String(presion));
  Serial.println("lluvia: " + String(lluvia));
  Serial.println("intensidad de lluvia: " + String(lluvia_intensidad));
  Serial.println("granizo: " + String(granizo));
  Serial.println("intensidad de granizo: " + String(granizo_intensidad));
  Serial.println("Status: " + String(msg.lastMessageStatus));
  Serial.println();
  Serial.println("---------------------------------------------MSG Sigfox-------------------------------------------------------");
  Serial.println("direccion del viento: " + String(msg.direccion));
  Serial.println("velocidad del viento: " + String(msg.velocidad));
  Serial.println("Temperatura ambiente: " + String(msg.temperatura));
  Serial.println("humedad relativa: " + String(msg.humedad));
  Serial.println("Presión atmosférica: " + String(msg.presion));
  Serial.println("lluvia: " + String(msg_2.lluvia));
  Serial.println("intensidad de lluvia: " + String(msg_2.intensidad_lluvia));
  Serial.println("granizo: " + String(msg_2.granizo));
  Serial.println("intensidad de granizo: " + String(msg_2.intensidad_granizo));
  Serial.println("Status: " + String(msg.lastMessageStatus));
  Serial.println();

}

void reboot() {
  NVIC_SystemReset();
  while (1);
}
