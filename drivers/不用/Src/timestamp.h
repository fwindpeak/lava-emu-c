#ifndef TIMESTAMP__H
#define TIMESTAMP__H

#include "rtc.h"
//mcg #include "globals.h"
#include "stm32f10x.h"  //mcg
#include <stdbool.h>    //mcg


enum TimeFormat {
  TF_HUMAN_SHORT,    // ex. 09/07/10-14:36:22.122044
  TF_ASCII_UNIX,     // ex. 1247028947.122044
  TF_ASCII_EXCEL,    // # days . frac days since 1900
};
typedef enum TimeFormat TimeFormat;

enum TimePrecision {
  TP_UNKNOWN            = 0x00,
  TP_DATE               = 0x01,
  TP_SECONDS            = 0x03,
  TP_MILLISECONDS       = 0x07,
  TP_MICROSECONDS       = 0x0F
};
typedef enum TimePrecision TimePrecision;


uint16_t format_rtc_time(char * buf, uint16_t buf_size, rtc_time * time, 
                         TimeFormat fmt, TimePrecision prec);
void display_time(rtc_time * time);
void display_cur_time(void);

#endif
