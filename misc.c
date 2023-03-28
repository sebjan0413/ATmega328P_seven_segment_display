/********************************************************************************
* misc.c: Innehåller diverse funktionsdefinitioner.
********************************************************************************/
#include "misc.h"

/********************************************************************************
* delay_ms: Genererar fördröjning mätt i millisekunder.
*
*           - delay_time_ms: Angiven fördröjningstid i millisekunder.
********************************************************************************/
void delay_ms(const uint16_t delay_time_ms)
{
   for (uint16_t i = 0; i < delay_time_ms; ++i)
   {
      _delay_ms(1);
   }
   return;
}

/********************************************************************************
* delay_us: Genererar fördröjning mätt i mikrosekunder.
*
*           - delay_time_us: Angiven fördröjningstid i mikrosekunder.
********************************************************************************/
void delay_us(const uint16_t delay_time_us)
{
   for (uint16_t i = 0; i < delay_time_us; ++i)
   {
      _delay_us(1);
   }
   return;
}

/********************************************************************************
* delay_ms_ptr: Genererar fördröjning mätt i millisekunder via en pekare.
*
*           - delay_time_um: Pekare till fördröjningstiden mätt i millisekunder.
********************************************************************************/
void delay_ms_ptr(const volatile uint16_t* delay_time_ms)
{
   for (uint16_t i = 0; i < *delay_time_ms; ++i)
   {
      _delay_ms(1);
   }
   return;
}

/********************************************************************************
* delay_us_ptr: Genererar fördröjning mätt i mikrosekunder via en pekare.
*
*           - delay_time_us: Pekare till fördröjningstiden mätt i mikrosekunder.
********************************************************************************/
void delay_us_ptr(const volatile uint16_t* delay_time_us)
{
   for (uint16_t i = 0; i < *delay_time_us; ++i)
   {
      _delay_us(1);
   }
   return;
}