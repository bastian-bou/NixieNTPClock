#ifndef NIXIE_CLOCK_H
#define NIXIE_CLOCK_H

#include <Arduino.h>

// Command anode of nixie valve
#define DOZ_H       32
#define UNIT_H      33
#define DOZ_M       25
#define UNIT_M      26
// Command of point in nixie valve
#define P_DOZ_H     27
#define P_UNIT_H    14
#define P_DOZ_M     12
#define P_UNIT_M    13
// Command quartet to send digit to nixie driver
#define A           16
#define B           17
#define C           18
#define D           19

#define STATE_MACHINE


#define TIMEOUT_BETWEEN_TWO_DIGITS_US   4
#define UP_NIXIE_DURATION_US            4000
#define SECOND_US                       1000000
#define NIXIE_ANIM_US                   500000

#define GET_TIME_US()                   ((uint64_t)esp_timer_get_time())

enum stateRefreshNixie {
    DOZENHOUR,
    UNITHOUR,
    DOZENMIN,
    UNITMIN,
    WAITING,
    DOTHOUR
};
enum dataDisplay {
    TIME,
    TEMP,
    MANUAL
};
enum testType {
    DOT,
    DIGITS_MULTIPLEX,
    DIGITS_NORMAL
};

class NixieClock {
    private:
        uint8_t hour;
        uint8_t min;
        uint8_t sec;
        int8_t  integer_temp;
        uint8_t dec_temp;
        uint64_t previous_get_time;
        uint64_t previous_sec;
        uint64_t previous_nixie_up_duration;
        stateRefreshNixie states;
        boolean is_nixie_on;

        void writeDigit(uint8_t digit);
        void refresh(dataDisplay type, uint8_t data[4]);
        void refresh(dataDisplay type);
    public:
        NixieClock();

        void setTime(uint8_t setHour, uint8_t setMin, uint8_t setSec);
        void setHour(uint8_t setHour);
        void setMin(uint8_t setMin);
        void setSec(uint8_t setSec);
        void setTemp(int8_t integer, uint8_t decimal);
        void setNixieOn();
        void setNixieOff();

        void testNixie(testType test);

        void resetAll();
        
        boolean showTime();
        void showTemp();
        void doWaitingAnim();
};


#endif