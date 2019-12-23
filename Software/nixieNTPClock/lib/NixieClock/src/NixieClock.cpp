#include "NixieClock.h"

/**
 * Brief: Creation of the object, configure pinout, variables initialization 
 */ 
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
    sec = min = hour = entierTemp = decTemp = 0;
    isTimeToRefreshTime = false;
    // Disable digit on nixie tubes
    isNixieOn = false;

    states = DOZENHOUR;
}

/**
 * Setter of all variables corresponding to the clock
 */
void NixieClock::setTime(uint8_t _hour, uint8_t _min, uint8_t _sec)
{
    hour = _hour;
    min = _min;
    sec = _sec;
    previousSec = esp_timer_get_time();
}

/**
 * Setter on hour
 */
void NixieClock::setHour(uint8_t _hour)
{
    hour = _hour;
}

/**
 * Setter on minute
 */
void NixieClock::setMin(uint8_t _min)
{
    min = _min;
}

/**
 * Setter on second
 */
void NixieClock::setSec(uint8_t _sec)
{
    sec = _sec;
    previousSec = esp_timer_get_time();
}

/**
 * Setter on all variables for temperature
 */
void NixieClock::setTemp(int8_t entier, uint8_t decimal)
{
    entierTemp = entier;
    decTemp = decimal;
    //after temperature, refresh clock with NTP server
    isTimeToRefreshTime = true;
}

/**
 * Enable digit on nixie tubes
 */
void NixieClock::setNixieOn()
{
    isNixieOn = true;
}

/**
 * Disable digit on nixie tubes, you can still command dot
 */
void NixieClock::setNixieOff()
{
    isNixieOn = false;
}

/**
 * Write digit on nixie driver (BCD -> 10seg)
 */
void NixieClock::writeDigit(uint8_t digit)
{
    uint8_t i;
    if (isNixieOn) {
        for (i = 0; i < 4; i++) {
            digitalWrite(i + A, ((digit >> i) & 1) ? HIGH : LOW);
        }
    } else {
        for (i = 0; i < 4; i++) {
            digitalWrite(i + A, HIGH);
        }
    }
}

/**
 * Refresh nixie tubes without specific data (only clock or temperature)
 */
void NixieClock::refresh(dataDisplay type)
{
    uint8_t data[4] = {14, 14, 14, 14};
    refresh(type, data);
}

/**
 * Refresh nixie tubes with specific data
 */
void NixieClock::refresh(dataDisplay type, uint8_t data[4])
{
    if (type == TEMP) {
        digitalWrite(P_UNIT_H, HIGH);
        if (entierTemp < 0) digitalWrite (P_DOZ_H, HIGH);
        else digitalWrite (P_DOZ_H, LOW);
    }
    // State machine
    switch (states)
    {
    case DOZENHOUR:
        if (esp_timer_get_time() - previousNixieUpDuration >= UP_NIXIE_DURATION_US) {
            previousNixieUpDuration = esp_timer_get_time();
            digitalWrite(UNIT_M, LOW);
//            NOP();
            if (type == MANUAL)
                writeDigit(data[0]);
            else
                // dozen of hour
                (type == TIME) ? writeDigit(hour / 10) : writeDigit((uint8_t)entierTemp / 10);
            digitalWrite(DOZ_H, HIGH);
            states = UNITHOUR;
        }
        break;
    case UNITHOUR:
        if (esp_timer_get_time() - previousNixieUpDuration >= UP_NIXIE_DURATION_US) {
            previousNixieUpDuration = esp_timer_get_time();
            digitalWrite(DOZ_H, LOW);
//            NOP();
            if (type == MANUAL)
                writeDigit(data[1]);
            else
                // unit of hour or temp
                (type == TIME) ? writeDigit(hour % 10) : writeDigit((uint8_t)entierTemp % 10);
            digitalWrite(UNIT_H, HIGH);
            states = DOZENMIN;
        }
        break;
    case DOZENMIN:
        if (esp_timer_get_time() - previousNixieUpDuration >= UP_NIXIE_DURATION_US) {
            previousNixieUpDuration = esp_timer_get_time();
            digitalWrite(UNIT_H, LOW);
//            NOP();
            if (type == MANUAL)
                writeDigit(data[2]);
            else
                // dozen of minute
                (type == TIME) ? writeDigit(min / 10) : writeDigit(decTemp / 10);
            digitalWrite(DOZ_M, HIGH);
            states = UNITMIN;
        }
        break;
    case UNITMIN:
        if (esp_timer_get_time() - previousNixieUpDuration >= UP_NIXIE_DURATION_US) {
            previousNixieUpDuration = esp_timer_get_time();
            digitalWrite(DOZ_M, LOW);
//            NOP();
            if (type == MANUAL)
                writeDigit(data[3]);
            else
                // unit of minute
                (type == TIME) ? writeDigit(min % 10) : writeDigit(decTemp % 10);
            digitalWrite(UNIT_M, HIGH);
            states = DOZENHOUR;
        }
        break;
    case WAITING:
        if (esp_timer_get_time() - previousNixieUpDuration >= TIMEOUT_BETWEEN_TWO_DIGITS_US) {
            previousNixieUpDuration = esp_timer_get_time();

        }
        break;
    default:
        break;
    }
}

/**
 * Put low level on all pins for nixie function
 */
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

/**
 * Update time and refresh nixie tubes
 */
boolean NixieClock::showTime()
{
    static boolean isRefreshed = false;
    //Update of not the clock with NTP
    if (((hour == 2) || (hour == 12)) && (isRefreshed == false)) {
        isRefreshed = true;
        return true;
    } else if (isTimeToRefreshTime) {
        isTimeToRefreshTime = false;
        return true;
    } else if (((hour == 3) || (hour == 13)) && (isRefreshed == true)){
        isRefreshed = false;
    }

    if (esp_timer_get_time() - previousSec >= SECOND_US) {
        // 1 second
        previousSec = esp_timer_get_time();
        sec++;
        // toggle unit hour comma (point)
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
    refresh(TIME);

    return false;
}

/**
 * refresh nixie tubes with temperature
 */
void NixieClock::showTemp()
{
    refresh(TEMP);
}

/**
 * Animation with dot on nixie tubes, chenillard like (ubuntu loading effect)
 */
void NixieClock::doWaitingAnim()
{
    static uint8_t position = 0;
    static boolean direction = true;

    if (esp_timer_get_time() - previousGetTime >= NIXIE_ANIM_US) {
        previousGetTime = esp_timer_get_time();
        switch (position) {
            case 0:
                digitalWrite(P_DOZ_H, direction ? HIGH : LOW);
            break;
            case 1:
                digitalWrite(P_UNIT_H, direction ? HIGH : LOW);
            break;
            case 2:
                digitalWrite(P_DOZ_M, direction ? HIGH : LOW);
            break;
            case 3:
                digitalWrite(P_UNIT_M, direction ? HIGH : LOW);
                direction = !direction;
            break;
        }

        position++;
        if (position > 3) position = 0;
    }
    refresh(MANUAL);
}

void NixieClock::testNixie(testType test)
{
    static uint8_t digitValue = 0;
    static uint8_t data[4] = {0, 0, 0, 0};
    static uint8_t position = 0;
    switch (test)
    {
    case DOT:
        doWaitingAnim();
        break;

    case DIGITS_MULTIPLEX:
        if (esp_timer_get_time() - previousGetTime >= NIXIE_ANIM_US) {
            previousGetTime = esp_timer_get_time();
            digitValue++;
            if (digitValue > 9) 
                digitValue = 0;
            data[0] = data[1] = data[2] = data[3] = digitValue;
        }
        refresh(MANUAL, data);
        break;

    case DIGITS_NORMAL:
        if (esp_timer_get_time() - previousGetTime >= NIXIE_ANIM_US) {
            previousGetTime = esp_timer_get_time();
            switch (position)
            {
            case 0:
                digitalWrite(UNIT_M, LOW);
                writeDigit(digitValue);
                digitalWrite(DOZ_H, HIGH);
                break;
            case 1:
                digitalWrite(DOZ_H, LOW);
                writeDigit(digitValue);
                digitalWrite(UNIT_H, HIGH);
                break;
            case 2:
                digitalWrite(UNIT_H, LOW);
                writeDigit(digitValue);
                digitalWrite(DOZ_M, HIGH);
                break;
            case 3:
                digitalWrite(DOZ_M, LOW);
                writeDigit(digitValue);
                digitalWrite(UNIT_M, HIGH);
                break;
            default:
                break;
            }
            digitValue++;
            if (digitValue > 9)
            {
                digitValue = 0;
                position++;
                if (position > 3)
                    position = 0;
            }
        }
        break;
    
    default:
        break;
    }
}