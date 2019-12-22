#include <Arduino.h>
#include "debug.h"

#include <NixieClock.h>

#include <OneWire.h>
#include <DallasTemperature.h>

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
// NTPclient object connected on France server (more accurate) 3600 for UTC +1
NTPClient timeClient(ntpUDP, "fr.pool.ntp.org", 3600, 0);

// SSID and password
// char wifi_ssid[] = "ssid";
// char wifi_pwd[]  = "password";

char wifi_ssid[] = "la_wifi_de_bastian";
char wifi_pwd[]  = "wifiDeBastian";

boolean isTimeGetTemp;

float temperature;


void init_IO() {

  sensors.begin();

}
/**
 * Return the temperature in celcius
 * return DEVICE_DISCONNECTED_C = -127 if there is an error
 **/
void showTemp() {
  if (isTimeGetTemp) {
    sensors.requestTemperatures();
    isTimeGetTemp = false;
    //temp in celcius
    temperature = sensors.getTempCByIndex(0);
    //set the temperature in nixie object
    nixie.setTemp((int8_t)temperature, (uint8_t)(temperature - (int8_t)temperature));
  }
  //refresh nixie tubes
  nixie.showTemp();
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
  Serial.begin(9600);
  debug_print("\n\n-----Start program-------\n\n\n");
#endif
  init_IO();

  isTimeGetTemp = true;

  WiFi.begin(wifi_ssid, wifi_pwd); // Connection to the network
  debug_print("Connecting to ");
  debug_print(wifi_ssid);
 
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    nixie.doWaitingAnim();
  }
 
  debug_print('\n');
  debug_print("Connection established!");
  debug_print("IP address:\t");
  debug_print(WiFi.localIP());
  debug_print('\n');

  nixie.resetAll();
  // Activate nixie tubes (digit)
  nixie.setNixieOn();

  timeClient.begin();
  // Get the time from NTP server
  timeClient.update();
  debug_print("Current time: ");
  debug_print(timeClient.getHours());debug_print(":");
  debug_print(timeClient.getMinutes());debug_print(":");
  debug_print(timeClient.getSeconds());debug_print("\n");
  // Set the time in Nixie object
  nixie.setTime(
    (uint8_t)timeClient.getHours(), 
    (uint8_t)timeClient.getMinutes(),
    (uint8_t)timeClient.getSeconds());
}

void loop() {
  
  if (isTouch()) {
    debug_print("Touch temp\n");
    showTemp();
  } else {
    isTimeGetTemp = true;
    if (nixie.showTime()) {
      getNtpTime();
    }
  }
}