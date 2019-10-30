#include <Arduino.h>

#include <NixieClock.h>

#include <OneWire.h>
#include <DallasTemperature.h>


//Temperature sensor 
#define TEMP        5
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

  init_IO();
}

void loop() {
  nixie.refreshTime();
  if (isTouch()) {
    float temp = getTemp();
    nixie.refreshTemp((int8_t)temp, (uint8_t)(temp - (int8_t)temp));
  }
}