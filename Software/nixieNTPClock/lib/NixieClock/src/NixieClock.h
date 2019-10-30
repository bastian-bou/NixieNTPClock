#ifndef H_NIXIE_CLOCK
#define H_NIXIE_CLOCK

#include <Arduino.h>

//Command anode of nixie valve
#define DIZ_H       1
#define UNIT_H      2
#define DIZ_M       3
#define UNIT_M      6
//Command of point in nixie valve
#define P_DIZ_H     7
#define P_UNIT_H    8
#define P_DIZ_M     9
#define P_UNIT_M    10
//Command quartet to send digit to nixie driver
#define A           11
#define B           12
#define C           13
#define D           14


#define REFRESH_TIMEOUT_US     40000

class NixieClock {
    private:
        uint8_t hour;
        uint8_t min;
        uint8_t sec;
        int64_t previousGetTime;
        int64_t previousSec;

        void writeDigit(uint8_t digit);
    public:
        NixieClock();
        void setTime(uint8_t _hour, uint8_t _min, uint8_t _sec);
        void setHour(uint8_t _hour);
        void setMin(uint8_t _min);
        void setSec(uint8_t sec);
        void resetAll();
        void refreshTime();
        void refreshTemp(int8_t entier, uint8_t decimal);
};


#endif