//------------------------------------------------------------------------------
//  Description:
//    Real Time Clock interface, incorporating a correction factor
//    Exposes rtc_time structure.
//    See timestamp.c for functions used to format rtc_time values.
//
//  Functions:
//    format_rtc_time, display_time, display_cur_time
//
//  Interrupts:
//    None
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#include "timestamp.h"
#include <time.h>
#include <stdio.h>

//------------------------------------------------------------------------------
//  Global Variables 
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  Private Defines 
//------------------------------------------------------------------------------
#define DIG(x) ((x) + '0')

//------------------------------------------------------------------------------
//  Private Variables 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Private Function Declarations 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Public Function Definitions 
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Function Name  : format_rtc_time
// Description    : Displays the given time
// Input          : buf - buffer to write output string
//                  buf_size - max # of characters to write into buf
//                  time - the rtc_time to format
//                  fmt - which predefined format to use (see below)
//                  prec - how much precision to use
// Output         : Non
// Return         : Number of characters written into buf
//------------------------------------------------------------------------------
uint16_t format_rtc_time(char * buf, uint16_t buf_size, rtc_time * time, TimeFormat fmt, TimePrecision prec)
{
  uint16_t buf_ptr = 0;
  int i;
  uint32_t scale;
  uint32_t seconds;
  uint32_t modsec; 
  uint32_t days_since_1900;
  uint64_t nanosec_of_day; 
  uint64_t day_frac;

  if (time->time_ref != TIMEREF_UNIX &&
      time->time_ref != TIMEREF_ARBITRARY)
  {
    return 0; // unsupported time reference, no bytes written
  }  

  if (fmt == TF_HUMAN_SHORT && time->time_ref == TIMEREF_UNIX)
  {
    // human readable, compact format
    if (prec >= TP_DATE && buf_size >= 8)
    {
      struct tm * timeinfo = gmtime((time_t*)&time->seconds);
      if (buf_size < 8)
        return 0;
      buf[buf_ptr++] = DIG((timeinfo->tm_year % 100) / 10);  // check whether this is actually 4-digit year!
      buf[buf_ptr++] = DIG(timeinfo->tm_year % 10);
      buf[buf_ptr++] = '/';
      buf[buf_ptr++] = DIG((timeinfo->tm_mon+1) / 10);
      buf[buf_ptr++] = DIG((timeinfo->tm_mon+1) % 10);
      buf[buf_ptr++] = '/';
      buf[buf_ptr++] = DIG((timeinfo->tm_mday) / 10);
      buf[buf_ptr++] = DIG((timeinfo->tm_mday) % 10);
      
      if (prec >= TP_SECONDS && buf_size >= 17)
      {
        buf[buf_ptr++] = '-';
        buf[buf_ptr++] = DIG((timeinfo->tm_hour) / 10);
        buf[buf_ptr++] = DIG((timeinfo->tm_hour) % 10);
        buf[buf_ptr++] = ':';
        buf[buf_ptr++] = DIG((timeinfo->tm_min) / 10);
        buf[buf_ptr++] = DIG((timeinfo->tm_min) % 10);
        buf[buf_ptr++] = ':';
        buf[buf_ptr++] = DIG((timeinfo->tm_sec) / 10);
        buf[buf_ptr++] = DIG((timeinfo->tm_sec) % 10);
      }

    }
  }
  else if (fmt == TF_ASCII_UNIX || (fmt == TF_HUMAN_SHORT && time->time_ref == TIMEREF_ARBITRARY))
  {
    if (prec >= TP_DATE && buf_size >= 10)
    {
      scale = 1000000000L;
      seconds = time->seconds;
      buf[buf_ptr++] = DIG(seconds/scale);
      for (i = 0; i < 9; i++) 
      {
        modsec = seconds % scale; 
        scale /= 10;
        buf[buf_ptr++] = DIG(modsec / scale);
      }
    }
  }
  else if (fmt == TF_ASCII_EXCEL)
  {
    // convert seconds since 1/1/1970 to days since 1/0/1900
    // (note, excel will be off by ~19 leap seconds from UTC,
    //  since the time base is actually TAI)
    
    if (buf_size >= 16)
    {
      // compute number of days:
      // divide by 86400 (sec per day)
      // add 25569, 1/1/1970 as excel serial date
      days_since_1900 = (time->seconds / 86400) + 25569;
  
      // compute nanoseconds of day
      nanosec_of_day = (((time->seconds % 86400) * 1000000ULL)
        + (uint64_t)(time->microsec)) * 1000ULL;
      
      // compute day fraction (in 1e-10 days)
      day_frac = nanosec_of_day / 8640;
      
      // output to buffer
      // 5-digit day number
      // .
      // 10-digit day fraction
      
      // 5-digit day number:
      buf[buf_ptr++] = DIG((days_since_1900 % 100000) / 10000 );
      buf[buf_ptr++] = DIG((days_since_1900 % 10000 ) / 1000  );
      buf[buf_ptr++] = DIG((days_since_1900 % 1000  ) / 100   );
      buf[buf_ptr++] = DIG((days_since_1900 % 100   ) / 10    );
      buf[buf_ptr++] = DIG((days_since_1900 % 10    )         );
      buf[buf_ptr++] = '.';
      
      scale = 1000000000ULL;
      buf[buf_ptr++] = DIG(day_frac/scale);
      for (i = 0; i < 9; i++) 
      {
        modsec = day_frac % scale; 
        scale /= 10;
        buf[buf_ptr++] = DIG(modsec / scale);
      }
    }
  }
  else
  {
    return 0;
  }
  
  if (fmt != TF_ASCII_EXCEL && 
      prec >= TP_MILLISECONDS && 
      buf_size >= buf_ptr+4)
  {
    buf[buf_ptr++] = '.';
    buf[buf_ptr++] = DIG((time->microsec         ) / 100000);
    buf[buf_ptr++] = DIG((time->microsec % 100000) / 10000 );
    buf[buf_ptr++] = DIG((time->microsec % 10000 ) / 1000  );

    if (prec >= TP_MICROSECONDS && buf_size >= buf_ptr+3)
    {
      buf[buf_ptr++] = DIG((time->microsec % 1000) / 100);
      buf[buf_ptr++] = DIG((time->microsec % 100 ) / 10 );
      buf[buf_ptr++] = DIG((time->microsec % 10  )      );
    }
  }
   
  
  return buf_ptr;
}

//------------------------------------------------------------------------------
// Function Name  : display_time
// Description    : Displays the given time in a default format
// Input          : None
// Output         : None
// Return         : None
//------------------------------------------------------------------------------
void display_time(rtc_time * time)
{
  char fmt_buf[26];
  
  uint16_t num_written = format_rtc_time(fmt_buf, 25, time, 
                                         TF_ASCII_EXCEL, TP_MILLISECONDS);
  fmt_buf[num_written] = '\0';
  
  if (num_written > 0)
  {
    printf("%s\r\n", fmt_buf);
  }
  else
  {
    printf("NO_TIME_FMT\r\n");
  } 
}

void display_cur_time(void)
{
    rtc_time now = get_time_rtc();
    display_time(&now);
}


// implementation for FatFs support
unsigned long get_fattime(void)
{
//  bit 31:25
//    Year from 1980 (0..127)
//  bit 24:21
//    Month (1..12)
//  bit 20:16
//    Day in month(1..31)
//  bit 15:11
//    Hour (0..23)
//  bit 10:5
//    Minute (0..59)
//  bit 4:0
//    Second / 2 (0..29) 
  
  uint32_t packed = 0x00000000;
  
  rtc_time now = get_time_rtc();
  if (now.time_ref == TIMEREF_UNIX)
  {
    struct tm * timeinfo = gmtime((time_t*)&now.seconds);
    packed |= ((timeinfo->tm_year - 80)   & 0x7F) << 25;
    packed |= ((timeinfo->tm_mon + 1)     & 0x0F) << 21;
    packed |= ((timeinfo->tm_mday)        & 0x1F) << 16;
    packed |= ((timeinfo->tm_hour)        & 0x1F) << 11;
    packed |= ((timeinfo->tm_min)         & 0x3F) << 5;
    packed |= ((timeinfo->tm_sec / 2)     & 0x1F) << 0;
  }

  return packed;
}

