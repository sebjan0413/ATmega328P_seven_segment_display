/********************************************************************************
* pwm.h: Innehåller drivrutiner för PWM-styrning av en godtycklig utenhet,
*        såsom en eller flera lysdioder.
********************************************************************************/
#ifndef PWM_H_
#define PWM_H_

/* Inkluderingsdirektiv: */
#include "misc.h"
#include "adc.h"

/********************************************************************************
* pwm: Strukt för PWM-kontrollers, som möjliggör PWM-styrning av en godtycklig 
*      utenhet, exempelvis en eller flera lysdioder implementerat via ett 
*      led-objekt eller en vektor innehållande pekare till multipla led-objekt. 
*      PWM-styrning kan ske via en analog insignal såsom en potentiometer eller
*      genom att direkt välja duty cycle.
********************************************************************************/
struct pwm
{
   struct adc input;               /* Analog inenhet, såsom en potentiometer. */
   uint16_t period_us;             /* Periodtid för PWM mätt i mikrosekunder. */
   void* output;                   /* Pekare till ansluten utenhet. */
   void (*output_high)(void* arg); /* Pekare till funktion för att tända ansluten utenhet. */
   void (*output_low)(void* arg);  /* Pekare till funktion för att släcka ansluten utenhet. */
   bool enabled;                   /* Enable-signal för kontroll av PWM-generering. */
};

/********************************************************************************
* pwm_init: Initierar PWM-kontroller för PWM-styrning av angiven utenhet via
*           angiven analog insignal. Som default är PWM-styrning aktiverat.
*
*           - self       : Pekare till PWM-kontrollern som ska initieras.
*           - input_pin  : Pekare till analog pin som utgör insignal.
*           - period_us  : Periodtid för PWM mätt i mikrosekunder.
*           - output     : Pekare till utenheten som ska styras med PWM.
*           - output_high: Funktionspekare för aktivering av angiven utenhet.
*           - output_low : Funktionspekare för inaktivering av angiven utenhet.
********************************************************************************/
void pwm_init(struct pwm* self, 
              uint8_t input_pin,
              const uint16_t period_us, 
              void* output,
              void* output_high, 
              void* output_low);

/********************************************************************************
* pwm_clear: Nollställer angiven PWM-kontroller.
*
*            - self: Pekare till PWM-kontrollern som ska nollställas.
********************************************************************************/
void pwm_clear(struct pwm* self);

/********************************************************************************
* pwm_enable: Aktiverar angiven PWM-kontroller så att ansluten utenhet kan
*             styras via PWM-generering.
*
*             - self: Pekare till PWM-kontrollern som ska aktiveras.
********************************************************************************/
static inline void pwm_enable(struct pwm* self)
{
   self->enabled = true;
   return;
}

/********************************************************************************
* pwm_disable: Inaktiverar angiven PWM-kontroller, vilket medför att
*              PWM-styrning av ansluten utenhet inte kan genomföras.
*
*
*              - self: Pekare till PWM-kontrollern som ska inaktiveras.
********************************************************************************/
static inline void pwm_disable(struct pwm* self)
{
   self->enabled = false;
   self->output_low(self->output);
   return;
}

/********************************************************************************
* pwm_toggle: Togglar aktivering av angiven PWM-kontroller.
*
*
*              - self: Pekare till PWM-kontrollern som ska togglas.
********************************************************************************/
static inline void pwm_toggle(struct pwm* self)
{
   if (self->enabled)
   {
      pwm_disable(self);
   }
   else
   {
      pwm_enable(self);
   }
   return;
}

/********************************************************************************
* pwm_run: Kör angiven PWM-kontroller under en period och styr ansluten utenhet
*          via avläsning av ansluten analog insignal, förutsatt att 
*          PWM-kontrollern är aktiverad.
*
*          - self: Pekare till PWM-kontrollern som ska köras.
********************************************************************************/
void pwm_run(struct pwm* self);

/********************************************************************************
* pwm_run_with_duty_cycle: Kör angiven PWM-kontroller under en period och styr 
*                          ansluten utenhet med angiven duty cycle, förutsatt 
*                          att PWM-kontrollern är aktiverad. Duty cycle måste
*                          anges som ett flyttal mellan 0 - 1, vilket motsvarar
*                          0 - 100 % duty cycle.
*
*         -                - self: Pekare till PWM-kontrollern som ska köras.
*                          - duty_cycle: Duty cycle, alltså andelen av aktuell
*                                        periodtid som ansluten utenhet ska
*                                        vara aktiverad, mellan 0 - 1.
********************************************************************************/
void pwm_run_with_duty_cycle(struct pwm* self, const double duty_cycle);

#endif /* PWM_H_ */