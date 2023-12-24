#if !(__has_include("params.h"))
#error "You need to add a params.h file in include folder with wifi_ssid and wifi_pwd const char*"
#endif

#include <Arduino.h>
#include "debug.h"
#include "params.h"

#include <NixieClock.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <WiFi.h>
#include "time.h"

//Temperature sensor 
#define TEMP           15
//Capacitive touch sensor
#define TOUCH          4
#define TOUCH_TRESHOLD 20

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(TEMP);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensor(&oneWire);
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

  sensor.begin();

}
/**
 * Get the temperature from the sensor (only once) and show temp on nixies
*/
void showTemp() {
  if (isTimeGetTemp) {
    sensor.requestTemperatures();
    isTimeGetTemp = false;
    //temp in celcius
    temperature = sensor.getTempCByIndex(0);
    //set the temperature in nixie object
    nixie.setTemp((int8_t)temperature, (uint8_t)((temperature - (int8_t)temperature) * 100));
  }
  //refresh nixie tubes
  nixie.showTemp();
}

/**
 * Test the capacitive button
 * @return 1 if someone touch the button, 0 if anyone touch it
*/
boolean isTouch()
{
  if (touchRead(TOUCH) < TOUCH_TRESHOLD) {
    return true;
  } else {
    return false;
  }
}

/**
 * Get time from sntp server (synchronization)
*/
void getNtpTime()
{
  struct tm timeInfo;

  getLocalTime(&timeInfo);
  nixie.setTime(
    (uint8_t)timeInfo.tm_hour, 
    (uint8_t)timeInfo.tm_min,
    (uint8_t)timeInfo.tm_sec);
}

/**
 * Arduino setup
*/
void setup()
{

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

/**
 * Arduino loop
*/
void loop()
{
  
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