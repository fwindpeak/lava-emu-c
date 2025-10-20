#include "lavasim.h"
#include "lvm.h"
#include "lcd.h"
#include "key.h"
#include "fonts.h"
#include "delay.h"
#include "rtc.h"
#include "display.h"
#include "ff.h"
#include "prtscr.h"

int main(void)
{
    lcd_init();
    key_init();
    font_init();
    lava_init();
    ff_init();
    PrtScr_Init();
    // lvm_main();
    lava_demo();
    return 0;
}
