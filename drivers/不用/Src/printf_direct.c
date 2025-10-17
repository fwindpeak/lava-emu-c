/*-----------------------------------------------------------------------------
 printf_direct.c  Defines putchar to direct printf's output, in this case to the debug console
 See http://www.rowley.co.uk/documentation/arm_1_7/hcc_customizing_putchar.htm for a faster way using a linker symbol.
 Author:  Albert McGilvra
 Sept 9, 2009
-----------------------------------------------------------------------------*/


/*****************************************************************************/
//Redirect putchar to Crossworks debug console

#include <__cross_studio_io.h>

int __putchar(int ch)
{
  return debug_putchar(ch);
}
/*****************************************************************************/

