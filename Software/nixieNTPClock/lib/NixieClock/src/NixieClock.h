#ifndef H_NIXIE_CLOCK
#define H_NIXIE_CLOCK

#include <Arduino.h>
#include "debug.h"

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


#define REFRESH_NIXIE_TIMEOUT_US    40000
#define UP_NIXIE_DURATION_US        1000
#define SECOND_US                   1000000

enum stateRefreshNixie {DOZENHOUR, UNITHOUR, DOZENMIN, UNITMIN, WAITING};
enum dataDisplay {TIME, TEMP};

class NixieClock {
    private:
        uint8_t hour;
        uint8_t min;
        uint8_t sec;
        int64_t previousGetTime;
        int64_t previousSec;
        int64_t previousNixieUpDuration;
        stateRefreshNixie states;

        void writeDigit(uint8_t digit);
        void resetAll();
        void refresh(dataDisplay type);
    public:
        NixieClock();
        void setTime(uint8_t _hour, uint8_t _min, uint8_t _sec);
        void setHour(uint8_t _hour);
        void setMin(uint8_t _min);
        void setSec(uint8_t sec);
        
        void refreshTime();
        void refreshTemp(int8_t entier, uint8_t decimal);
};


#endif