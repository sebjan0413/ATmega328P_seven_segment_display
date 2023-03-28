/********************************************************************************
* timer.h: Innehåller drivrutiner för interruptbaserade timerkretsar via 
*          strukten timer samt associerade funktioner. Dessa timerkretsar 
*          fungerar också utmärkt att använda som räknare.
********************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_

/* Inkluderingsdirektiv: */
#include "misc.h"

/********************************************************************************
* timer_sel: Enumeration för val av timerkrets.
********************************************************************************/
enum timer_sel
{
   TIMER_SEL_0,   /* Timer 0. */
   TIMER_SEL_1,   /* Timer 1. */
   TIMER_SEL_2,   /* Timer 2. */
   TIMER_SEL_NONE /* Timer ospecificerad. */
};

/********************************************************************************
* timer: Strukt för implementering av interruptbaserade timerkretsar, som
*        vid behov kan användas som räknare.
********************************************************************************/
struct timer
{
   volatile uint32_t counter; /* 32-bitars räknare. */
   uint32_t max_count;        /* Maxvärde som uppräkning ska ske till. */
   volatile uint8_t* timsk;   /* Pekare till maskregister för aktivering av avbrott. */
   uint8_t timsk_bit;         /* Bit för aktivering av avbrott i motsvarande maskregister. */
   enum timer_sel timer_sel;  /* Val av timerkrets. */
};

/********************************************************************************
* timer_init: Initierar ny timerkrets med angiven tid mätt i millisekunder.
*             Om timern ska användas som räknare för att räkna upp till ett
*             specifikt maxvärde bör funktionen timer_set_max_count anropas
*             direkt efter initieringen.
*
*             - self     : Pekare till timern som ska initieras.
*             - timer_sel: Val av timerkrets.
*             - time_ms  : Tiden timern ska sättas på mätt i millisekunder.
********************************************************************************/
void timer_init(struct timer* self, 
                const enum timer_sel timer_sel, 
                const double time_ms);

/********************************************************************************
* timer_clear: Genomför total nollställning av angiven timerkrets.
*
*              - self: Pekare till timern som ska nollställas.
********************************************************************************/
void timer_clear(struct timer* self);

/********************************************************************************
* timer_enable_interrupt: Aktiverar timergenererat avbrott, som äger rum när
*                         timern räknar upp till overflow eller specificerat max.
*
*                         Timer 0 samt Timer 2 aktiveras i Normal Mode, medan
*                         Timer 1 aktiveras i CTC Mode med uppräkning till 256,
*                         vilket gör att tiden mellan varje timergenererat
*                         avbrott är samma oavsett använd timerkrets.
*
*                         Avbrottsvektorer för timerkretsarna deklareras nedan:
*
*                         Timerkrets     Avbrottsvektor
*                           Timer 0     TIMER0_OVF_vect
*                           Timer 1     TIMER1_COMPA_vect
*                           Timer 2     TIMER2_OVF_vect
*
*                         - self: Pekare till timern som timergenererat
*                                 avbrott ska aktiveras på.
********************************************************************************/
static inline void timer_enable_interrupt(struct timer* self)
{
   *(self->timsk) |= (1 << self->timsk_bit);
   return;
}

/********************************************************************************
* timer_disable_interrupt: Inaktiverar timergenererat avbrott på angiven timer.
*
*                          - self: Pekare till timern som timergenererat
*                                  avbrott ska inaktiveras på.
********************************************************************************/
static inline void timer_disable_interrupt(struct timer* self)
{
   *(self->timsk) &= ~(1 << self->timsk_bit);
   return;
}

/********************************************************************************
* timer_toggle_interrupt: Togglar aktivering av timergenererat avbrott på
*                         angiven timer.
*
*                         - self: Pekare till timern som aktivering av 
*                                 timergenererat avbrott ska togglas på.
********************************************************************************/
void timer_toggle_interrupt(struct timer* self);

/********************************************************************************
* timer_interrupt_enabled: Indikerar ifall timergenererat avbrott är aktiverat
*                          på angiven timer.
*
*                          - self: Pekare till timern vars avbrottstillstånd
*                                  ska kontrolleras.
********************************************************************************/
static inline bool timer_interrupt_enabled(const struct timer* self)
{
   return (*(self->timsk) & (1 << self->timsk_bit));
}

/********************************************************************************
* timer_count: Räknar upp angiven timer.
*
*              - self: Pekare till timern som ska räknas upp.
********************************************************************************/
static inline void timer_count(struct timer* self)
{
   self->counter++;
   return;
}

/********************************************************************************
* timer_elapsed: Indikerar ifall angiven timer har löpt ut genom att returnera
*                true eller false. Ifall timern har löpt ut nollställs räknaren
*                inför nästa uppräkning.
*
*                - self: Pekare till timern som ska kontrolleras.
********************************************************************************/
bool timer_elapsed(struct timer* self);

/********************************************************************************
* timer_reset_counter: Nollställer räknaren på angiven timer.
*
*                      - self: Pekare till timern vars räknare ska nollställas.
********************************************************************************/
static inline void timer_reset_counter(struct timer* self)
{
   self->counter = 0;
   return;
}

/********************************************************************************
* timer_reset: Återställer angiven timer till startläget.
*
               - self: Pekare till timern som ska återställas.
********************************************************************************/
void timer_reset(struct timer* self);

/********************************************************************************
* timer_set_new_time: Sätter ny tid på angiven timerkrets.
*
*                    - self   : Pekare till timern vars tid ska uppdateras.
*                    - time_ms: Tiden timern ska sättas på mätt i millisekunder.
********************************************************************************/
void timer_set_new_time(struct timer* self, 
                        const double time_ms);

/********************************************************************************
* timer_set_new_max_count: Sätter nytt maxvärde för uppräkning av timern när
*                          denna ska användas som en räknare.
*
*                          - self   : Pekare till timern.
*                          - max_count: Maxvärde för uppräkningen.
********************************************************************************/
static inline void timer_set_max_count(struct timer* self,
                                       const uint32_t max_count)
{
   self->max_count = max_count;
   return;
}

#endif /* TIMER_H_ */