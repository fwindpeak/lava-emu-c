//------------------------------------------------------------------------------
//  Description:
//    Real Time Clock interface, incorporating a correction factor
//    Exposes rtc_time structure.
//    See timestamp.c for functions used to format rtc_time values.
//
//  Functions:
//    get_time_rtc
//    
//    Primarily for use by sync_time.c:
//    set_time_rtc, subtract_time_rtc, update_rtc_error_correction,
//    get/set/reset_rtc_error_correction
//
//  Interrupts:
//    1PPS external interrupt --> sync_received()
//
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  Includes Files
//------------------------------------------------------------------------------
#include "rtc.h"
#include <time.h>
#include "stm32f10x_pwr.h"  //mcg
#include "stm32f10x_bkp.h"  //mcg
#include "stm32f10x_rtc.h"  //mcg

//------------------------------------------------------------------------------
// Private Defines
//------------------------------------------------------------------------------
#define DEFAULT_RTC_CYCLES_1000X   62499000


//------------------------------------------------------------------------------
// Globals
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Private Variables
//------------------------------------------------------------------------------
int32_t div_offset = 0;
uint8_t time_ref = TIMEREF_INVALID;
uint32_t rtc_cycles_1000x = DEFAULT_RTC_CYCLES_1000X;

uint16_t cur_prescaler = 0;

uint8_t rtc_initialized = 0;


// error correction variables
rtc_time ref_uncorrected;    // counter values for reference point
rtc_time ref_corrected;      // equivalent corrected time for reference point
int32_t correction_factor; // the amount to adjust the uncorrected time since 
                            // last_reference by, (delta µs)/(elapsed sec) 


//------------------------------------------------------------------------------
// Private Function Declarations
//------------------------------------------------------------------------------
rtc_time correct_time(rtc_time * uncorrected);
rtc_time get_time_rtc_uncorrected(void);
rtc_time offset_time_rtc(rtc_time * start, int32_t delta_microsec);



//------------------------------------------------------------------------------
// Public Function Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Name  : init_RTC
// Description    : Initializes 
// Input          : None
// Output         : None
// Return         : None
//------------------------------------------------------------------------------
void init_RTC(void)
{
  // Enable PWR and BKP clocks 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  // Allow access to BKP Domain 
  PWR_BackupAccessCmd(ENABLE);

  // Reset Backup Domain 
  BKP_DeInit();

  // Enable LSE 
//  RCC_LSEConfig(RCC_LSE_ON);
  
  // Wait till LSE is ready 
//  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

  // Select LSE as RTC Clock Source 
//  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); // Watch crystal
  
  // Select high-speed external oscillator (divided by 128) as clk source
  RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);

  // Enable RTC Clock 
  RCC_RTCCLKCmd(ENABLE);

  // Wait for RTC registers synchronization 
  RTC_WaitForSynchro();

  // Wait until last write operation on RTC registers has finished 
  RTC_WaitForLastTask();
  
  // Set RTC Alarm 
  // Wake up from standby when Time is 23:59:59.999
  //RTC_SetAlarm(0x5265C00); // total seconds in hex
  
  // Wait until last write operation on RTC registers has finished 
//  RTC_WaitForLastTask();

  // Enable the RTC Second interrupt
//  RTC_ITConfig(RTC_IT_SEC, ENABLE);
  
  // Wait until last write operation on RTC registers has finished 
//  RTC_WaitForLastTask();  
  
  // Enable the RTC Alarm interrupt  
//  RTC_ITConfig(RTC_IT_ALR, ENABLE);

  // Wait until last write operation on RTC registers has finished 
//  RTC_WaitForLastTask();

  // Set RTC prescaler
  
  // LSE OSC 32.768-KHz (Watch crystal)
  // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) 
//  RTC_SetPrescaler(32767); // RTC period to 1 sec 
//  RTC_SetPrescaler(33);    // RTC period to 1ms     
  
  // HSE OSC 8-MHz (8Mhz Crystal)
  // (8MHz/128)/62499 = 1 Hz
  RTC_SetPrescaler(rtc_cycles_1000x/1000);
  cur_prescaler = (rtc_cycles_1000x/1000);

  // Wait until last write operation on RTC registers has finished 
  RTC_WaitForLastTask();  
  
  div_offset = 0;
  time_ref = TIMEREF_ARBITRARY;
  reset_rtc_time(ref_uncorrected);
  reset_rtc_time(ref_corrected);
  correction_factor = 0;
  
  rtc_initialized = 1;
}

//------------------------------------------------------------------------------
// Function Name  : get_time_rtc
// Description    : Returns the current time, with error correction applied
//                  if known (requires GPS 1PPS time sync)
// Input          : None
// Output         : None
// Return         : Current time as an rtc_time (see rtc.h)
//------------------------------------------------------------------------------
rtc_time get_time_rtc(void)
{
  rtc_time now = get_time_rtc_uncorrected();
  now = correct_time(&now);
  return now;  
}

//------------------------------------------------------------------------------
// Function Name  : set_time_rtc
// Description    : Sets the current RTC to the time specified 
// Input          : time to set
// Output         : RTC counter
// Return         : None
//------------------------------------------------------------------------------
void set_time_rtc(rtc_time * time)
{
/*!!mcg
  if (debug_reg & DBG_MSG_TIMING)
  {
    printf("                Old counter: %i(div 0x%04X)\r\n", 
         RTC_GetCounter(), RTC_GetDivider());
  }
end mcg */

  RTC_SetCounter(time->seconds);
  
  // note - since it is not possible to set the RTC divider, we instead
  // just store an offset so it looks like we're changing it to calls of  
  // get_time_rtc_uncorrected
  div_offset = time->microsec / 16;
  time_ref = time->time_ref;
  if (time_ref == TIMEREF_RELATIVE)
  {
    time_ref = TIMEREF_ARBITRARY; // don't automatically add time
  }
  RTC_WaitForLastTask(); // wait for the write operation to complete
}

//------------------------------------------------------------------------------
// Function Name  : subtract_time_rtc
// Description    : Determines the time elapsed between start and end
// Input          : end, start (must have same time_ref)
// Output         : None
// Return         : elapsed time (end-start), with time_ref = TIMEREF_RELATIVE
//                  if arguments not valid, then time_ref = TIMEREF_INVALID
//------------------------------------------------------------------------------
rtc_time subtract_time_rtc(rtc_time * end, rtc_time * start)
{
  rtc_time result;
    uint32_t max_us;
    uint32_t max_seconds;
    uint32_t min_us;
  if (end->time_ref == start->time_ref)
  {
    rtc_time * max;
    rtc_time * min;

    int positive = 0;
    if (end->seconds > start->seconds)
      positive = 1;
    else if (end->seconds == start->seconds)
    {
      if (end->microsec >= start->microsec)
        positive = 1;
    }
   
    if (positive)     { max = end;    min = start;  }
    else              { max = start;  min = end;    }
    
    max_us = max->microsec;
    max_seconds = max->seconds;
    min_us = min->microsec;
    
    if (max_us < min_us)
    {
      // carry from seconds
      max_seconds--;
      max_us += 1000000;
    }
    
    result.microsec = max_us - min_us;
    result.seconds = max_seconds - min->seconds; 
    result.sign = positive ? 1 : -1;
      
    if (end->time_ref == TIMEREF_INVALID)
      result.time_ref = TIMEREF_INVALID;
    else
      result.time_ref = TIMEREF_RELATIVE;
  }
  else
  {
    reset_rtc_time(result);
  }
  
  return result;
}


//------------------------------------------------------------------------------
// Function Name  : time
// Description    : implementation of standard c library's time function
//                  only returns whole seconds, or -1 if unknown
//                  use get_time_rtc for more accuracy
// Input          : t (pointer to time_t to set to the current time, or NULL)
// Output         : None
// Return         : current time (unix seconds)
//------------------------------------------------------------------------------
time_t time(time_t *t)
{
  rtc_time now = get_time_rtc();
  time_t sec;
  if (now.time_ref == TIMEREF_UNIX)
    sec = (time_t)(now.seconds);
  else
    sec = (time_t) (-1);
  
  if (t)
  {
    *t = sec;
  }
  return sec;
}

//------------------------------------------------------------------------------
// Function Name  : reset_rtc_error_correction
// Description    : Initializes error correction to have no effect
// Input          : None
// Output         : modifies error correction vars
// Return         : None
//------------------------------------------------------------------------------
void reset_rtc_error_correction(void)
{
  set_rtc_error_correction(0);
}

//------------------------------------------------------------------------------
// Function Name  : set_rtc_error_correction
// Description    : Sets the error correction factor to the new value, in 
//                  units of microseconds per second,
//                  and re-synchronizes the reference points (so corrections
//                  are only applied from this point on)
// Input          : new correction factor
// Output         : modifies error correction vars
// Return         : None
//------------------------------------------------------------------------------
void set_rtc_error_correction(int32_t new_correction_factor)
{
  // set the last reference point to now, so that the new value doesn't 
  // propagate backward through all previous time
  ref_uncorrected = get_time_rtc_uncorrected();
  ref_corrected = correct_time(&ref_uncorrected);
  correction_factor = new_correction_factor;
}

//------------------------------------------------------------------------------
// Function Name  : get_rtc_error_correction
// Description    : Gets the current error correction factor, 
//                  in microseconds per second
// Input          : None
// Output         : None
// Return         : current error correction factor
//------------------------------------------------------------------------------
int32_t get_rtc_error_correction(void)
{
  return correction_factor;
}


//------------------------------------------------------------------------------
// Private func.  : correct_time
// Description    : Takes an uncorrected time (derived from current RTC timer 
//                  value) and applies the correction_factor:
// 
//  corrected = 
//        ((uncorrected - ref_uncorrected) * scaling) + ref_corrected
//------------------------------------------------------------------------------
rtc_time correct_time(rtc_time * uncorrected)
{
  int32_t denom;
  rtc_time result = *uncorrected;
  int32_t offset_microsec;
  if (correction_factor != 0)
  {
    // determine (uncorrected) time elapsed since reference
    rtc_time elapsed = subtract_time_rtc(uncorrected, &ref_uncorrected);
    
    if (elapsed.time_ref != TIMEREF_INVALID)
    {
      int32_t microsec_elapsed = elapsed.seconds * 1000000LL + 
        elapsed.microsec;
      if (elapsed.sign < 0)
        microsec_elapsed = -microsec_elapsed;
      
      // determine number of 16 microsecond intervals to correct by
      // based on the correction factor

      if (correction_factor > -500000 && correction_factor < 500000)
        denom = (1000+correction_factor/1000);
      else
        denom = 1000;
      offset_microsec = (int64_t)((int64_t)correction_factor * (int64_t)microsec_elapsed / 1000) / denom;
      
      // apply scale factor
      result = offset_time_rtc(&ref_corrected, microsec_elapsed + offset_microsec);
    }
  }
  return result;
}

//------------------------------------------------------------------------------
// Private func.  : get_time_rtc_uncorrected
// Description    : Returns the current time, without applying correction_factor
//------------------------------------------------------------------------------
rtc_time get_time_rtc_uncorrected(void)
{
  static rtc_time result; // static for speed
  static uint32_t rtc_count2;
  static int32_t div;
  
  result.seconds = RTC_GetCounter();
  div = RTC_GetDivider();
  rtc_count2 = RTC_GetCounter();
  
  if (result.seconds < rtc_count2)
  {
    // count incremented, so div only valid if it's low (it counts down)
    // otherwise, it has already rolled over, and a more accurate time
    // can be obtained by setting it low
    div = 0;
  }
  
  div -= div_offset;
  while (div < 0)
  {
    div += rtc_cycles_1000x/1000;
    result.seconds++;
  }
  result.microsec = 16 * (rtc_cycles_1000x/1000 - div);
  
  result.time_ref = time_ref;
  result.sign = 1;
  
  return result;
}


//------------------------------------------------------------------------------
// Private func.  : offset_time_rtc
// Description    : Returns an rtc_time which is start + delta_microseconds
//------------------------------------------------------------------------------
rtc_time offset_time_rtc(rtc_time * start, int32_t delta_microsec)
{
  rtc_time result;
  uint8_t new_timeref     = start->time_ref;
  int8_t new_sign         = start->sign;
  uint32_t new_seconds    = start->seconds;
  int32_t new_microsec    = start->microsec + 
    (new_sign > 0 ? delta_microsec : -delta_microsec);
  
  while (new_microsec >= 1000000)
  {
    // microseconds rolled over (positive)
    new_microsec -= 1000000;
    new_seconds++;
    if (new_seconds == 0) // check for rollover in seconds
    {
      new_timeref = TIMEREF_INVALID;
    }
  }
  
  if (new_microsec < 0)
  {
    // microseconds rolled over (negative)
    new_microsec = -new_microsec; // sign handled using result.sign field
    if (new_seconds == 0)  // check whether we have room to decrement seconds
    {
      if (new_timeref == TIMEREF_RELATIVE) // if relative time, negative is okay
      {
        new_sign = -new_sign;
      }
      else
      {
        new_timeref = TIMEREF_INVALID;    // other times must be positive
      }
    }
    else
    {
      new_seconds--;
    }
  }
    
  result.time_ref     = new_timeref;
  result.sign         = new_sign;
  result.seconds      = new_seconds;
  result.microsec     = new_microsec;
  
  return result;
}


