//------------------------------------------------------------------------------
// rtc Header
//------------------------------------------------------------------------------
#ifndef RTC_h
#define RTC_h

//mcg #include "globals.h"
#include "stm32f10x.h"  //mcg
#include <stdbool.h>    //mcg


#ifndef TIMEREF_CONSTS
#define TIMEREF_CONSTS

// time should not be used
#define TIMEREF_INVALID   (uint8_t)0x00

// for a time difference
#define TIMEREF_RELATIVE  (uint8_t)0x01

// arbitrary reference (since timer startup, etc.)
#define TIMEREF_ARBITRARY (uint8_t)0x02

// relative to Jan 1st 1970, 00:00 UTC
#define TIMEREF_UNIX      (uint8_t)0x03

#endif


struct rtc_time
{
  uint32_t seconds;
  uint32_t microsec;
  uint8_t  time_ref;
  int8_t  sign;
};
typedef struct rtc_time rtc_time;

#define reset_rtc_time(t) do {t.seconds = t.microsec = 0; \
                               t.time_ref = TIMEREF_INVALID; \
                               t.sign = 1; } while (0);



//------------------------------------------------------------------------------
// Globals
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------
void init_RTC(void);
rtc_time get_time_rtc(void);
void set_time_rtc(rtc_time * time);
rtc_time subtract_time_rtc(rtc_time * end, rtc_time * start);
void reset_rtc_error_correction(void);
int32_t get_rtc_error_correction(void);
void set_rtc_error_correction(int32_t new_correction_factor);



#endif // RTC_h
