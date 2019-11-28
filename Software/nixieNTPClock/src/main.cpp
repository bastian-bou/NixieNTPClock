#include <Arduino.h>

#include <NixieClock.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include "debug.h"

#include <WiFi.h>

#include <NTPClient.h>
#include <WiFiUdp.h>


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

// ntpServer object
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "fr.pool.ntp.org", 0, 0);


char wifi_ssid[] = "ssid";
char wifi_pwd[]  = "password";


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

void getNtpTime() {
  timeClient.update();
  nixie.setTime(
    (uint8_t)timeClient.getHours(), 
    (uint8_t)timeClient.getMinutes(),
    (uint8_t)timeClient.getSeconds());
}

void setup() {

#ifdef DEBUG
  Serial.begin(115200);
  debug_print("-----Start program-------\n\n\n");
#endif
  init_IO();

  WiFi.begin(wifi_ssid, wifi_pwd);             // Connect to the network
  debug_print("Connecting to ");
  debug_print(wifi_ssid);

  nixie.setNixieOn();
 
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    nixie.doWaitingAnim();
  }
 
  debug_print('\n');
  debug_print("Connection established!");
  debug_print("IP address:\t");
  debug_print(WiFi.localIP());
  debug_print('\n');

  nixie.resetAll();

  timeClient.begin();
}

void loop() {
  if (nixie.refreshTime()) {
    getNtpTime();
  }
  if (isTouch()) {
    debug_print("Touch temp\n");
    float temp = getTemp();
    nixie.refreshTemp((int8_t)temp, (uint8_t)(temp - (int8_t)temp));
  }
}