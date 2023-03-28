/********************************************************************************
* led.h: Innehåller drivrutiner för lysdioder ocn andra digitala utportar via
*        strukten led samt associerade funktioner. 
********************************************************************************/
#ifndef LED_H_
#define LED_H_

/* Inkluderingsdirektiv: */
#include "misc.h"

/********************************************************************************
* led: Strukt för implementering av lysdioder och andra digitala utportar.
********************************************************************************/
struct led
{
   uint8_t pin;              /* Lysdiodens pin-nummer på aktuell I/O-port. */
   volatile uint8_t* ddr;    /* Pekare till datariktningsregister. */
   volatile uint8_t* output; /* Pekare till dataregister (för tändning och släckning). */
   volatile uint8_t* input;  /* Pekare till pinregister (för läsning av insignaler). */
};

/********************************************************************************
* led_init: Initierar ny lysdiod på angiven pin.
*
*           - self: Pekare till lysdioden som ska initieras.
*           - pin : Lysdiodens pin-nummer på Arduino Uno, exempelvis 8.
*                   Alternativt kan motsvarande port-nummer på ATmega328P
*                   anges, exempelvis B0 för pin 8 eller D2 för pin 2.
********************************************************************************/
void led_init(struct led* self,
              const uint8_t pin);

/********************************************************************************
* led_clear: Nollställer lysdiod samt motsvarande pin.
*
*            - self: Pekare till lysdioden som ska nollställas.
********************************************************************************/
void led_clear(struct led* self);

/********************************************************************************
* led_on: Tänder angiven lysdiod.
*
*         - self: Pekare till lysdioden som ska tändas.
********************************************************************************/
static inline void led_on(struct led* self)
{
   *(self->output) |= (1 << self->pin);
   return;
}

/********************************************************************************
* led_off: Släcker angiven lysdiod.
*
*          - self: Pekare till lysdioden som ska släckas.
********************************************************************************/
static inline void led_off(struct led* self)
{
   (*self->output) &= ~(1 << self->pin);
   return;
}

/********************************************************************************
* led_toggle: Togglar utsignalen på angiven lysdiod. Om lysdioden är släckt vid
*             anropet så tänds den. På samma sätt gäller att om lysdioden är
*             tänd vid anropet så släcks den.
*
*             - self: Pekare till lysdioden vars utsignal ska togglas.
********************************************************************************/
static inline void led_toggle(struct led* self)
{
   (*self->input) = (1 << self->pin);
   return;
}

/********************************************************************************
* led_enabled: Indikerar ifall angiven lysdiod är tänd.
*
*              - self: Pekare till lysdioden vars tillstånd ska kontrolleras.
********************************************************************************/
static inline bool led_enabled(const struct led* self)
{
   return ((*self->input) & (1 << self->pin));
}


/********************************************************************************
* led_blink: Blinkar lysdiod en gång med angiven blinkhastighet.
*
*            - self          : Pekare till lysdioden som ska blinkas.
*            - blink_speed_ms: Blinkhastigheten mätt i millisekunder.
********************************************************************************/
void led_blink(struct led* self,
               const uint16_t blink_speed_ms);

#endif /* LED_H_ */