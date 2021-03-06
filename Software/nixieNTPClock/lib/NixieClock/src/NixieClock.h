#ifndef H_NIXIE_CLOCK
#define H_NIXIE_CLOCK

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

#define getTimeUs()   ((uint64_t)esp_timer_get_time())

enum stateRefreshNixie {DOZENHOUR, UNITHOUR, DOZENMIN, UNITMIN, WAITING, DOTHOUR};
enum dataDisplay {TIME, TEMP, MANUAL};
enum testType {DOT, DIGITS_MULTIPLEX, DIGITS_NORMAL};

class NixieClock {
    private:
        uint8_t hour;
        uint8_t min;
        uint8_t sec;
        int8_t  entierTemp;
        uint8_t decTemp;
        uint64_t previousGetTime;
        uint64_t previousSec;
        uint64_t previousNixieUpDuration;
        stateRefreshNixie states;
        boolean isNixieOn;
        boolean isTimeToRefreshTime;

        void writeDigit(uint8_t digit);
        void refresh(dataDisplay type, uint8_t data[4]);
        void refresh(dataDisplay type);
    public:
        NixieClock();

        void setTime(uint8_t _hour, uint8_t _min, uint8_t _sec);
        void setHour(uint8_t _hour);
        void setMin(uint8_t _min);
        void setSec(uint8_t sec);
        void setTemp(int8_t entier, uint8_t decimal);

        void setNixieOn();
        void setNixieOff();

        void testNixie(testType test);

        void resetAll();
        
        boolean showTime();
        void showTemp();
        void doWaitingAnim();
};


#endif