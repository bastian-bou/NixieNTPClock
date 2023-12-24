#include <Arduino.h>
#include "debug.h"
#include "params.h"

#include <NixieClock.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <WiFi.h>
#include "time.h"


//Temperature sensor 
#define TEMP  15
//Tactile sensor
#define TOUCH 4


// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(TEMP);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
// Create nixie object
NixieClock nixie;

// sntp parameters
const char * ntp_server = "fr.pool.ntp.org";
const long   gmt_offset_sec = 3600;
const int    daylight_offset_sec = 3600;
const char * timezone = "CET-1CEST,M3.5.0,M10.5.0/3"; // Timezone of Paris -> https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

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
    nixie.setTemp((int8_t)temperature, (uint8_t)((temperature - (int8_t)temperature) * 100));
  }
  //refresh nixie tubes
  nixie.showTemp();
}

boolean isTouch() {
  if (touchRead(TOUCH) < 20) return true;
  else return false;
}

void getNtpTime() {
  struct tm timeInfo;

  getLocalTime(&timeInfo);
  nixie.setTime(
    (uint8_t)timeInfo.tm_hour, 
    (uint8_t)timeInfo.tm_min,
    (uint8_t)timeInfo.tm_sec);
}

void setup() {

#ifdef DEBUG
  Serial.begin(9600);
  debug_print("\n\n-----Start program-------\n\n\n");
#endif
  init_IO();

  isTimeGetTemp = true;
  /* If we touch the capacitive button (temp) at the begining, we start
  ** the nixie test loop
  ** We need to restart the device to stop this loop
  */
  // delay(500);
  // if (isTouch())
  // {
  //   int64_t previousUs = getTimeUs();
  //   testType test = DOT;
  //   nixie.resetAll();
  //   // Activate nixie tubes (digit)
  //   nixie.setNixieOn();

  //   debug_print("Start test on Nixie Clock\n");

  //   while (1) {
  //     nixie.testNixie(test);
  //     // Every 20 seconds, we change the test
  //     if ((getTimeUs() - previousUs) >= 20000000) {
  //       previousUs = getTimeUs();
  //       if (test == DOT) test = DIGITS_MULTIPLEX;
  //       else if (test == DIGITS_MULTIPLEX) test = DIGITS_NORMAL;
  //       else if (test == DIGITS_NORMAL) test = DOT;
  //     }
  //   }
  // }

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

  // Configure the sntp server
  configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);
  // Add the timezone to it
  setenv("TZ", timezone, 1);
  tzset();

  struct tm timeInfo;
  getLocalTime(&timeInfo);
  debug_print("Current time: ");
  debug_print(timeInfo.tm_hour);debug_print(":");
  debug_print(timeInfo.tm_min);debug_print(":");
  debug_print(timeInfo.tm_sec);debug_print("\n");
  // Set the time in Nixie object
  nixie.setTime(
    (uint8_t)timeInfo.tm_hour, 
    (uint8_t)timeInfo.tm_min,
    (uint8_t)timeInfo.tm_sec);
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