#include "NixieClock.h"

/**
 * Creation of the object, configure pinout, variables initialization
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

    previous_nixie_up_duration = previous_sec = previous_get_time = GET_TIME_US();
    sec = min = hour = integer_temp = dec_temp = 0;
    // Disable digit on nixie tubes
    is_nixie_on = false;
    // Set first state for state machine
    states = DOZENHOUR;
}

/**
 * Setter of all clock variables
 * @param setHour To set the hour value (i.e. from NTP)
 * @param setMin To set the minute value (i.e. from NTP)
 * @param setSec To set the second value (i.e. from NTP)
 */
void NixieClock::setTime(uint8_t setHour, uint8_t setMin, uint8_t setSec)
{
    hour = setHour;
    min = setMin;
    sec = setSec;
    // Update current tick time
    previous_sec = GET_TIME_US();
}

/**
 * Setter on hour
 * @param setHour To set the hour value (i.e. from NTP)
 */
void NixieClock::setHour(uint8_t setHour)
{
    hour = setHour;
}

/**
 * Setter on minute
 * @param setMin To set the minute value (i.e. from NTP)
 */
void NixieClock::setMin(uint8_t setMin)
{
    min = setMin;
}

/**
 * Setter on second
 * @param setSec To set the second value (i.e. from NTP)
 */
void NixieClock::setSec(uint8_t setSec)
{
    sec = setSec;
    previous_sec = GET_TIME_US();
}

/**
 * Setter on all temperature variables
 * @param integer The integer part of the temperature
 * @param decimal The decimal part of the temperature
 */
void NixieClock::setTemp(int8_t integer, uint8_t decimal)
{
    integer_temp = integer;
    dec_temp = decimal;
}

/**
 * Enable digit on nixie tubes
 */
void NixieClock::setNixieOn()
{
    is_nixie_on = true;
}

/**
 * Disable digit on nixie tubes, you can still command dot
 */
void NixieClock::setNixieOff()
{
    is_nixie_on = false;
}

/**
 * Write digit on nixie driver (BCD -> 10seg)
 * @param digit Value from 0 to 9
 */
void NixieClock::writeDigit(uint8_t digit)
{
    uint8_t i;
    if (is_nixie_on) {
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
 * @param type Type of data to display (time, temperature, ...)
 */
void NixieClock::refresh(dataDisplay type)
{
    uint8_t data[4] = {14, 14, 14, 14};
    refresh(type, data);
}

/**
 * Refresh nixie tubes with specific data
 * @param type Type of data to display (time, temperature, ...)
 * @param data Data in case of MANUAL type
 */
void NixieClock::refresh(dataDisplay type, uint8_t data[4])
{
    if (type == TEMP) {
        digitalWrite(P_UNIT_H, HIGH);
        if (integer_temp < 0) digitalWrite(P_DOZ_H, HIGH);
        else digitalWrite(P_DOZ_H, LOW);
    }
    // State machine
    switch (states)
    {
    case DOZENHOUR:
        if ((GET_TIME_US() - previous_nixie_up_duration) >= UP_NIXIE_DURATION_US) {
            previous_nixie_up_duration = GET_TIME_US();
            digitalWrite(UNIT_M, LOW);
            NOP();
            if (type != MANUAL)
                // dozen of hour
                (type == TIME) ? writeDigit(hour / 10) : writeDigit((uint8_t)integer_temp / 10);
            else
                writeDigit(data[0]);
            digitalWrite(DOZ_H, HIGH);
            states = UNITHOUR;
        }
        break;
    case UNITHOUR:
        if ((GET_TIME_US() - previous_nixie_up_duration) >= UP_NIXIE_DURATION_US) {
            previous_nixie_up_duration = GET_TIME_US();
            digitalWrite(DOZ_H, LOW);
            NOP();
            if (type != MANUAL)
                // unit of hour or temp
                (type == TIME) ? writeDigit(hour % 10) : writeDigit((uint8_t)integer_temp % 10);
            else
                writeDigit(data[1]);
            digitalWrite(UNIT_H, HIGH);
            states = DOZENMIN;
        }
        break;
    case DOZENMIN:
        if ((GET_TIME_US() - previous_nixie_up_duration) >= UP_NIXIE_DURATION_US) {
            previous_nixie_up_duration = GET_TIME_US();
            digitalWrite(UNIT_H, LOW);
            NOP();
            if (type != MANUAL)
                // dozen of minute
                (type == TIME) ? writeDigit(min / 10) : writeDigit(dec_temp / 10);
            else
                writeDigit(data[2]);
            digitalWrite(DOZ_M, HIGH);
            states = UNITMIN;
        }
        break;
    case UNITMIN:
        if ((GET_TIME_US() - previous_nixie_up_duration) >= UP_NIXIE_DURATION_US) {
            previous_nixie_up_duration = GET_TIME_US();
            digitalWrite(DOZ_M, LOW);
            NOP();
            if (type != MANUAL)
                // unit of minute
                (type == TIME) ? writeDigit(min % 10) : writeDigit(dec_temp % 10);
            else
                writeDigit(data[3]);
            digitalWrite(UNIT_M, HIGH);
            states = DOZENHOUR;
        }
        break;
    case WAITING:
        if ((GET_TIME_US() - previous_nixie_up_duration) >= TIMEOUT_BETWEEN_TWO_DIGITS_US) {
            previous_nixie_up_duration = GET_TIME_US();
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
 * @return If it's a new hour, return true to update time (i.e. with NTP)
 */
boolean NixieClock::showTime()
{
    boolean isRefreshed = false;

    if ((GET_TIME_US() - previous_sec) >= SECOND_US) {
        // 1 second
        previous_sec = GET_TIME_US();
        sec++;
        // toggle unit hour comma (point)
        digitalWrite(P_UNIT_H, digitalRead(P_UNIT_H) ? LOW : HIGH);
        if (sec > 59) {
            sec = 0;
            min++;
        }
        if (min > 59) {
            min = 0;
            //Update the clock with NTP
            isRefreshed = true;
        }
    }
    refresh(TIME);

    return isRefreshed;
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

    if ((GET_TIME_US() - previous_get_time) >= NIXIE_ANIM_US) {
        previous_get_time = GET_TIME_US();
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

/**
 * Test function for nixie clock with animations
 * @param test Type of animation for test
*/
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
        if ((GET_TIME_US() - previous_get_time) >= NIXIE_ANIM_US) {
            previous_get_time = GET_TIME_US();
            digitValue++;
            if (digitValue > 9) 
                digitValue = 0;
            data[0] = data[1] = data[2] = data[3] = digitValue;
        }
        refresh(MANUAL, data);
        break;

    case DIGITS_NORMAL:
        if ((GET_TIME_US() - previous_get_time) >= NIXIE_ANIM_US) {
            previous_get_time = GET_TIME_US();
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