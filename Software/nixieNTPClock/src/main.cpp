#include <Arduino.h>

#include <NixieClock.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include "debug.h"


//Temperature sensor 
#define TEMP        15
//Tactile sensor
#define TOUCH       4


// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(TEMP);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Create nixie object
NixieClock nixie;


void init_IO() {

  sensors.begin();

}
/**
 * Return the temperature in celcius
 * return DEVICE_DISCONNECTED_C = -127 if there is an error
 **/
float getTemp() {
  sensors.requestTemperatures();
  //return temp in celcius
  return  sensors.getTempCByIndex(0);
}

boolean isTouch() {
  if (touchRead(TOUCH) < 20) return true;
  else return false;
}

void setup() {

#ifdef DEBUG
  Serial.begin(115200);
  debug_print("-----Start program-------\n\n\n");
#endif
  init_IO();
}

void loop() {
  nixie.refreshTime();
  if (isTouch()) {
    debug_print("Touch temp\n");
    float temp = getTemp();
    nixie.refreshTemp((int8_t)temp, (uint8_t)(temp - (int8_t)temp));
  }
}