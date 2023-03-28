/********************************************************************************
* serial.c: Innehåller funktionsdefinitioner för implementering av seriell
*           överföring via USART.
********************************************************************************/
#include "serial.h"

/********************************************************************************
* serial_init: Initierar USART för seriell överföring med angiven baud rate,
*              där default sätts till 9600 kbps (kilobits/sekund). USART 
*              konfigureras till asynkron överföring med åtta bitar i taget,
*              utan stoppbit.
*
*              - baud_rate_kbps: Överföringshastigheten, dvs. antalet bitar som 
'                                transmitteras per sekund (default = 9600 kbps).
********************************************************************************/
void serial_init(const uint32_t baud_rate_kbps)
{
   static bool serial_initialized = false;
   if (serial_initialized) return;

   UCSR0B = (1 << TXEN0);
   UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);

   if (baud_rate_kbps == 0 || baud_rate_kbps == 9600)
   {
      UBRR0 = 103;
   }
   else
   {
      UBRR0 = (uint8_t)(F_CPU / (16.0 * baud_rate_kbps) - 1 + 0.5);  
   }

   UDR0 = '\r';
   serial_initialized = true;
   return;
}

/********************************************************************************
* serial_print_string: Skriver ut text via seriell överföring.
*
*                      - s: Pekare till det textstycke som ska skrivas ut.
********************************************************************************/
void serial_print_string(const char* s)
{
   for (const char* i = s; *i; ++i)
   {
      serial_print_char(*i);

      if (*i == '\n')
      {
         serial_print_char('\r');
      }
   }
   return;
}

/********************************************************************************
* serial_print_integer: Skriver ut ett signerat heltal via seriell överföring.
*
*                      - number: Heltalet som ska skrivas ut.
********************************************************************************/
void serial_print_integer(const int32_t number)
{
   char s[20] = { '\0' };
   sprintf(s, "%ld", number);
   serial_print_string(s);
   return;
}

/********************************************************************************
* serial_print_unsigned: Skriver ut ett osignerat heltal via seriell överföring.
*
*                        - number: Heltalet som ska skrivas ut.
********************************************************************************/
void serial_print_unsigned(const uint32_t number)
{
   char s[20] = { '\0' };
   sprintf(s, "%lu", number);
   serial_print_string(s);
   return;
}

/********************************************************************************
* serial_print_double: Skriver ut ett flyttal med upp till två decimaler 
*                      via seriell överföring.
*
*                      - number: Flyttalet som ska skrivas ut.
********************************************************************************/
void serial_print_double(const double number)
{
   const int32_t integer = (int32_t)number;
   int32_t decimal;
   char s[20] = { '\0' };
   
   if (integer >= 0)
   {
      decimal = (int32_t)((number - integer) * 100 + 0.5);
   }
   else
   {
      decimal = (int32_t)((integer - number) * 100 + 0.5);
   }

   sprintf(s, "%ld.%ld", integer, decimal);
   serial_print_string(s);
   return;
}

/********************************************************************************
* serial_print_char: Skriver ut ett enskilt tecken via seriell överföring.
*
*                    - character: Det tecken som ska skrivas ut.
********************************************************************************/
void serial_print_char(const char character)
{
   while ((UCSR0A & (1 << UDRE0)) == 0);
   UDR0 = character;
   return;
}