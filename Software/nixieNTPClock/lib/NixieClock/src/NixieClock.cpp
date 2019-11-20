#include "NixieClock.h"
#include "debug.h"

NixieClock::NixieClock()
{
    pinMode(DOZ_H, OUTPUT);
    pinMode(UNIT_H, OUTPUT);
    pinMode(DOZ_M, OUTPUT);
    pinMode(UNIT_M, OUTPUT);
    pinMode(P_DOZ_H, OUTPUT);
    pinMode(P_UNIT_H, OUTPUT);
    pinMode(P_DOZ_M, OUTPUT);
    pinMode(P_UNIT_M, OUTPUT);
    pinMode(A, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(C, OUTPUT);
    pinMode(D, OUTPUT);

    resetAll();

    previousNixieUpDuration = previousSec = previousGetTime = esp_timer_get_time();
    sec = min = hour = 0;

    states = DOZENHOUR;
}

void NixieClock::setTime(uint8_t _hour, uint8_t _min, uint8_t _sec)
{
    hour = _hour;
    min = _min;
    sec = _sec;
    previousSec = esp_timer_get_time();
}

void NixieClock::setHour(uint8_t _hour)
{
    hour = _hour;
}

void NixieClock::setMin(uint8_t _min)
{
    min = _min;
}

void NixieClock::setSec(uint8_t _sec)
{
    sec = _sec;
    previousSec = esp_timer_get_time();
}

void NixieClock::writeDigit(uint8_t digit)
{
    for (uint8_t i = 0; i < 4; i++) {
        digitalWrite(i + A, ((digit >> i) & 1) ? HIGH : LOW);
    }
}

void NixieClock::refresh(dataDisplay type)
{
#ifndef STATE_MACHINE

    digitalWrite(DOZ_H, HIGH);
    writeDigit(hour / 10); // dozen of hour
    digitalWrite(DOZ_H, LOW);

    digitalWrite(UNIT_H, HIGH);
    writeDigit(hour % 10); // unit of hour
    digitalWrite(UNIT_H, LOW);

    digitalWrite(DOZ_M, HIGH);
    writeDigit(min / 10); // dozen of minute
    digitalWrite(DOZ_M, LOW);

    digitalWrite(UNIT_M, HIGH);
    writeDigit(min % 10); // unit of minute
    digitalWrite(UNIT_M, LOW);

#else

    // State machine
    switch (states)
    {
    case DOZENHOUR:
        if (esp_timer_get_time() - previousNixieUpDuration >= UP_NIXIE_DURATION_US) {
            previousNixieUpDuration = esp_timer_get_time();
            digitalWrite(DOZ_H, LOW);
            digitalWrite(UNIT_H, HIGH);
            writeDigit(hour % 10); // unit of hour
            states = UNITHOUR;
        }
        break;
    case UNITHOUR:
        if (esp_timer_get_time() - previousNixieUpDuration >= UP_NIXIE_DURATION_US) {
            previousNixieUpDuration = esp_timer_get_time();
            digitalWrite(UNIT_H, LOW);
            digitalWrite(DOZ_M, HIGH);
            writeDigit(min / 10); // dozen of minute
            states = DOZENMIN;
        }
        break;
    case DOZENMIN:
        if (esp_timer_get_time() - previousNixieUpDuration >= UP_NIXIE_DURATION_US) {
            previousNixieUpDuration = esp_timer_get_time();
            digitalWrite(DOZ_M, LOW);
            digitalWrite(UNIT_M, HIGH);
            writeDigit(min % 10); // unit of minute
            states = UNITMIN;
        }
        break;
    case UNITMIN:
        if (esp_timer_get_time() - previousNixieUpDuration >= UP_NIXIE_DURATION_US) {
            previousNixieUpDuration = esp_timer_get_time();
            digitalWrite(UNIT_M, LOW);
            states = WAITING;
        }
        break;
    case WAITING:
        if (esp_timer_get_time() - previousNixieUpDuration >= REFRESH_NIXIE_TIMEOUT_US) {
            previousNixieUpDuration = esp_timer_get_time();
            digitalWrite(DOZ_H, HIGH);
            writeDigit(hour / 10); // dozen of hour
            states = DOZENHOUR;
        }
        break;
    default:
        break;
    }
#endif
}

void NixieClock::resetAll() 
{
    digitalWrite(DOZ_H, LOW);
    digitalWrite(UNIT_H, LOW);
    digitalWrite(DOZ_M, LOW);
    digitalWrite(UNIT_M, LOW);
    digitalWrite(P_DOZ_H, LOW);
    digitalWrite(P_UNIT_H, LOW);
    digitalWrite(P_DOZ_M, LOW);
    digitalWrite(P_UNIT_M, LOW);
    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
    digitalWrite(C, LOW);
    digitalWrite(D, LOW);
}


void NixieClock::refreshTime()
{
#ifndef STATE_MACHINE
    if (esp_timer_get_time() - previousGetTime >= REFRESH_NIXIE_TIMEOUT_US) {
        previousGetTime = esp_timer_get_time();
        // we work in µs
        if (previousGetTime - previousSec >= SECOND_US) {
            // 1 second
            previousSec = previousGetTime;
            sec++;
            digitalWrite(P_UNIT_H, digitalRead(P_UNIT_H) ? LOW : HIGH);
            if (sec >= 59) {
                sec = 0;
                min++;
                debug_print("1 MINUTE\n");
            }
            if (min >= 59) {
                min = 0;
                hour++;
                debug_print("1 HOUR\n");
            }
            if (hour >= 24) {
                hour = 0;
                debug_print("1 DAY\n");
            }
        }
        refresh(TIME);
    }
#else

    if (esp_timer_get_time() - previousSec >= SECOND_US) {
        // 1 second
        previousSec = esp_timer_get_time();
        sec++;
        // toggle unit hour comma (point)
        digitalWrite(P_UNIT_H, digitalRead(P_UNIT_H) ? LOW : HIGH);
        if (sec >= 59) {
            sec = 0;
            min++;
//            debug_print("1 MINUTE\n");
        }
        if (min >= 59) {
            min = 0;
            hour++;
//            debug_print("1 HOUR\n");
        }
        if (hour >= 24) {
            hour = 0;
//            debug_print("1 DAY\n");
        }
        refresh(TIME);
    }

    
#endif
}

void NixieClock::refreshTemp(int8_t entier, uint8_t decimal)
{

    digitalWrite(P_UNIT_H, HIGH);
    if (entier < 0) digitalWrite (P_DOZ_H, HIGH);
    else digitalWrite (P_DOZ_H, LOW);

    digitalWrite(DOZ_H, HIGH);
    writeDigit((uint8_t)entier / 10); // 
    digitalWrite(DOZ_H, LOW);

    digitalWrite(UNIT_H, HIGH);
    writeDigit((uint8_t)entier % 10); //
    digitalWrite(UNIT_H, LOW);

    digitalWrite(DOZ_M, HIGH);
    writeDigit(decimal / 10); // 
    digitalWrite(DOZ_M, LOW);

    digitalWrite(UNIT_M, HIGH);
    writeDigit(decimal % 10); //
    digitalWrite(UNIT_M, LOW);
}