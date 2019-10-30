#include "NixieClock.h"

NixieClock::NixieClock()
{
    pinMode(DIZ_H, OUTPUT);
    pinMode(UNIT_H, OUTPUT);
    pinMode(DIZ_M, OUTPUT);
    pinMode(UNIT_M, OUTPUT);
    pinMode(P_DIZ_H, OUTPUT);
    pinMode(P_UNIT_H, OUTPUT);
    pinMode(P_DIZ_M, OUTPUT);
    pinMode(P_UNIT_M, OUTPUT);
    pinMode(A, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(C, OUTPUT);
    pinMode(D, OUTPUT);

    resetAll();

    previousSec = previousGetTime = esp_timer_get_time();
    sec = min = hour = 0;
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

void NixieClock::resetAll() 
{
    digitalWrite(DIZ_H, LOW);
    digitalWrite(UNIT_H, LOW);
    digitalWrite(DIZ_M, LOW);
    digitalWrite(UNIT_M, LOW);
    digitalWrite(P_DIZ_H, LOW);
    digitalWrite(P_UNIT_H, LOW);
    digitalWrite(P_DIZ_M, LOW);
    digitalWrite(P_UNIT_M, LOW);
    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
    digitalWrite(C, LOW);
    digitalWrite(D, LOW);
}


void NixieClock::refreshTime()
{
    if (esp_timer_get_time() - previousGetTime >= REFRESH_TIMEOUT_US) {
        previousGetTime = esp_timer_get_time();
        // we work in µs
        if (previousGetTime - previousSec >= 1000000) {
            // 1 second
            previousSec = previousGetTime;
            sec++;
            digitalWrite(P_UNIT_H, digitalRead(P_UNIT_H) ? LOW : HIGH);
            if (sec >= 59) {
                sec = 0;
                min++;
            }
            if (min >= 59) {
                min = 0;
                hour++;
            }
            if (hour >= 24) {
                hour = 0;
            }
        }
        digitalWrite(DIZ_H, HIGH);
        writeDigit(hour / 10); // dizaine d'heure
        digitalWrite(DIZ_H, LOW);

        digitalWrite(UNIT_H, HIGH);
        writeDigit(hour % 10); //
        digitalWrite(UNIT_H, LOW);

        digitalWrite(DIZ_M, HIGH);
        writeDigit(min / 10); // dizaine de minute
        digitalWrite(DIZ_M, LOW);

        digitalWrite(UNIT_M, HIGH);
        writeDigit(min % 10); //
        digitalWrite(UNIT_M, LOW);
    }
}

void NixieClock::refreshTemp(int8_t entier, uint8_t decimal)
{

    digitalWrite(P_UNIT_H, HIGH);
    if (entier < 0) digitalWrite (P_DIZ_H, HIGH);
    else digitalWrite (P_DIZ_H, LOW);

    digitalWrite(DIZ_H, HIGH);
    writeDigit((uint8_t)entier / 10); // 
    digitalWrite(DIZ_H, LOW);

    digitalWrite(UNIT_H, HIGH);
    writeDigit((uint8_t)entier % 10); //
    digitalWrite(UNIT_H, LOW);

    digitalWrite(DIZ_M, HIGH);
    writeDigit(decimal / 10); // 
    digitalWrite(DIZ_M, LOW);

    digitalWrite(UNIT_M, HIGH);
    writeDigit(decimal % 10); //
    digitalWrite(UNIT_M, LOW);
}