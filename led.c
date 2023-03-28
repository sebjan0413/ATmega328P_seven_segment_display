/********************************************************************************
* led.h: Innehåller funktionsdefinitioner för implementering av lysdioder och
*        andra digitala utportar via strukten led.
********************************************************************************/
#include "led.h"

/********************************************************************************
* led_init: Initierar ny lysdiod på angiven pin.
*
*           - self: Pekare till lysdioden som ska initieras.
*           - pin : Lysdiodens pin-nummer på Arduino Uno, exempelvis 8.
*                   Alternativt kan motsvarande port-nummer på ATmega328P
*                   anges, exempelvis B0 för pin 8 eller D2 för pin 2.
********************************************************************************/
void led_init(struct led* self,
              const uint8_t pin)
{
   if (pin >= 0 && pin <= 7)
   {
      self->pin = pin;
      self->ddr = &DDRD;
      self->output = &PORTD;
      self->input = &PIND;
   }
   else if (pin >= 8 && pin <= 13)
   {
      self->pin = pin - 8;
      self->ddr = &DDRB;
      self->output = &PORTB;
      self->input = &PINB;
   }
   else if (pin >= 14 && pin <= 19)
   {
      self->pin = pin - 14;
      self->ddr = &DDRC;
      self->output = &PORTC;
      self->input = &PINC;
   }

   (*self->ddr) |= (1 << self->pin);
   return;
}

/********************************************************************************
* led_clear: Nollställer lysdiod samt motsvarande pin.
*
*            - self: Pekare till lysdioden som ska nollställas.
********************************************************************************/
void led_clear(struct led* self)
{
   *(self->ddr) &= ~(1 << self->pin);
   *(self->output) &= ~(1 << self->pin);

   self->pin = 0;
   self->ddr = 0;
   self->output = 0;
   self->pin = 0;
   return;
}

/********************************************************************************
* led_blink: Blinkar lysdiod en gång med angiven blinkhastighet.
*
*            - self          : Pekare till lysdioden som ska blinkas.
*            - blink_speed_ms: Blinkhastigheten mätt i millisekunder.
********************************************************************************/
void led_blink(struct led* self,
               const uint16_t blink_speed_ms)
{
   led_toggle(self);
   delay_ms(blink_speed_ms);
   return;
}
 