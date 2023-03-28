/********************************************************************************
* led_vector.h: Innehåller funktionalitet för implementering av vektorer för 
*               för lagring av pekare till multipla lysioder via strukten 
*               led_vector samt associerade funktioner, vilket möjliggör enkel
*               styrning av lysdioder och andra digitala utportar.
*
*               Lysdioder kan läggas till dynamiskt eller genom att en pekare 
*               till en statisk array innehållande lysdiodspekare passeras.
********************************************************************************/
#ifndef LED_VECTOR_H_
#define LED_VECTOR_H_

/* Inkluderingsdirektiv: */
#include "misc.h"
#include "led.h"

/********************************************************************************
* led_vector: Dynamisk vektor för lagring av pekare till led-objekt, vilket
*             möjliggör enkel styrning av lysdioder och andra digitala utportar.
********************************************************************************/
struct led_vector
{
   struct led** leds; /* Pekare till array innehållande lysdiodspekare. */
   size_t size;       /* Vektorns storlek, dvs. antalet befintliga lysdiodspekare. */
};

/********************************************************************************
* led_vector_init: Initierar angiven vektor till tom vid start.
*
*                  - self: Pekare till vektorn som ska initieras.
********************************************************************************/
static inline void led_vector_init(struct led_vector* self)
{
   self->leds = 0;
   self->size = 0;
   return;
}

/********************************************************************************
* led_vector_clear: Tömmer och nollställer angiven vektor.
*
*                   - self: Pekare till vektorn som ska tömmas.
********************************************************************************/
static inline void led_vector_clear(struct led_vector* self)
{
   free(self->leds);
   led_vector_init(self);
   return;
}

/********************************************************************************
* led_vector_begin: Returnerar en pekare till det första elementet i angiven
*                   vektor. Ifall vektorn är tom returneras null.
*
*                  - self: Pekare till vektorn.
********************************************************************************/
static inline struct led** led_vector_begin(const struct led_vector* self)
{
   return self->leds;
}

/********************************************************************************
* led_vector_end: Returnerar en pekare till adressen direkt efter det sista
*                 befintliga elementet i angiven vektor. Ifall vektorn är tom
*                 returneras null.
*
*                  - self: Pekare till vektorn.
********************************************************************************/
static inline struct led** led_vector_end(const struct led_vector* self)
{
   return self->leds + self->size;
}

/********************************************************************************
* led_vector_last: Returnerar en pekare till det sista befintliga elementet i
*                  angiven vektor. Ifall vektorn är tom returneras null.
*
*                  - self: Pekare till vektorn.
********************************************************************************/
static inline struct led** led_vector_last(const struct led_vector* self)
{
   return self->size > 0 ? led_vector_end(self) - 1 : 0;
}

/********************************************************************************
* led_vector_resize: Ändrar storleken på angiven vektor så att den efter
*                    omallokering rymmer angivet antal lysdiodspekare, som kan
*                    tilldelas direkt via index i stället för en push-operation.
*                    Vid misslyckad minnesallokering returneras felkod 1.
*                    Annars om omallokeringen lyckas så returneras 0.
*
*                    - self    : Pekare till vektorn vars storlek ska ändras.
*                    - new_size: Vektorns nya storlek.
********************************************************************************/
int led_vector_resize(struct led_vector* self, 
                      const size_t new_size);

/********************************************************************************
* led_vector_push: Lägger till en pekare till en ny lysdiod längst bak i angiven
*                  vektor. Vid misslyckad minnesallokering returneras felkod 1.
*                  Annars om push-operationen lyckas returneras 0.
*
*                  - self   : Pekare till vektorn som ska tilldelas.
*                  - new_led: Pekare till den nya lysdiod som ska läggas till.
********************************************************************************/
int led_vector_push(struct led_vector* self, 
                    struct led* new_led);

/********************************************************************************
* led_vector_pop: Tar bort eventuellt lysdiodspekare i angiven vektor genom
*                 att minska dess storlek med ett. Vid misslyckad omallokering
*                 returneras felkod 1, annars 0.
*
*                 - self: Pekare till vektorn vars sista element ska tas bort.
********************************************************************************/
int led_vector_pop(struct led_vector* self);

/********************************************************************************
* led_vector_on: Tänder samtliga lysdioder lagrade i angiven vektor.
*
*                - self: Pekare till vektorn vars lysdioder ska tändas.
********************************************************************************/
void led_vector_on(struct led_vector* self);

/********************************************************************************
* led_vector_off: Släcker samtliga lysdioder lagrade i angiven vektor.
*
*                 - self: Pekare till vektorn vars lysdioder ska släckas.
********************************************************************************/
void led_vector_off(struct led_vector* self);

/********************************************************************************
* led_vector_toggle: Togglar samtliga lysdioder lagrade i angiven vektor. 
*
*                    - self: Pekare till vektorn vars lysdioder ska togglas.
********************************************************************************/
void led_vector_toggle(struct led_vector* self);

/********************************************************************************
* led_vector_blink_collectively: Genomför kollektiv (synkroniserad) blinkning
*                                av samtliga lysdioder lagrade i angiven vektor.
*
*                                - self          : Pekare till vektorn vars
*                                                  lysdioder ska blinkas.
*                                - blink_speed_ms: Lysdiodernas blinkhastighet 
*                                                  mätt i millisekunder.
********************************************************************************/
void led_vector_blink_collectively(struct led_vector* self,
                                   const uint16_t blink_speed_ms);

/********************************************************************************
* led_vector_blink_sequentially: Genomför sekventiell blinkning av samtliga 
*                                lysdioder lagrade i angiven vektor. Därmed
*                                blinkar lysdioderna i en sekvens en efter en.
*
*                                - self          : Pekare till vektorn vars
*                                                  lysdioder ska blinkas.
*                                - blink_speed_ms: Lysdiodernas blinkhastighet
*                                                  mätt i millisekunder.
********************************************************************************/
void led_vector_blink_sequentially(struct led_vector* self,
                                   const uint16_t blink_speed_ms);

#endif /* LED_VECTOR_H_ */