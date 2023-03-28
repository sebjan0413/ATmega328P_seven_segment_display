/********************************************************************************
* pwm.c: Innehåller drivrutiner för PWM-styrning av en godtycklig utenhet,
*        såsom en eller flera lysdioder.
********************************************************************************/
#include "pwm.h"

/* Statiska funktioner: */
static inline void pwm_run_cycle(struct pwm* self);

/********************************************************************************
* pwm_init: Initierar PWM-kontroller för PWM-styrning av angiven utenhet via
*           angiven analog insignal.
*
*           - self       : Pekare till PWM-kontrollern som ska initieras.
*           - input_pin  : Pekare till analog pin som utgör insignal.
*           - period_us  : Periodtid för PWM mätt i mikrosekunder.
*           - output     : Pekare till utenheten som ska styras med PWM.
*           - output_high: Funktionspekare för aktivering av angiven utenhet.
*           - output_low : Funktionspekare för inaktivering av angiven utenhet.
********************************************************************************/
void pwm_init(struct pwm* self,
              const uint8_t input_pin,
              const uint16_t period_us,
              void* output,
              void* output_high,
              void* output_low)
{
   adc_init(&self->input, input_pin);
   self->period_us = period_us;
   self->output = output;
   self->output_high = output_high;
   self->output_low = output_low;
   self->enabled = true;
   return;
}

/********************************************************************************
* pwm_clear: Nollställer angiven PWM-kontroller.
*
*            - self: Pekare till PWM-kontrollern som ska nollställas.
********************************************************************************/
void pwm_clear(struct pwm* self)
{
   adc_clear(&self->input);
   self->period_us = 0;
   self->output = 0;
   self->output_high = 0;
   self->output_low = 0;
   self->enabled = false;
   return;
}

/********************************************************************************
* pwm_run: Kör angiven PWM-kontroller under en period och styr ansluten utenhet,
*          förutsatt att PWM-kontrollern är aktiverad.
*
*          - self: Pekare till PWM-kontrollern som ska köras.
********************************************************************************/
void pwm_run(struct pwm* self)
{
   if (!self->enabled) return;
   adc_get_pwm_values(&self->input, self->period_us);
   pwm_run_cycle(self);
   return;
}

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
void pwm_run_with_duty_cycle(struct pwm* self, 
                             const double duty_cycle)
{
   if (!self->enabled || duty_cycle < 0 || duty_cycle > 1) return;
   self->input.pwm_on_us = (uint16_t)(self->period_us * duty_cycle + 0.5);
   self->input.pwm_off_us = self->period_us - self->input.pwm_on_us;
   pwm_run_cycle(self);
   return;
}

/********************************************************************************
* pwm_run_cycle: Kör utenhet ansluten till angiven PWM-kontroller under en 
*                PWM-period med befintliga PWM-värden.
*
*                - self: Pekare till PWM-kontrollern vars anslutna utenhet
*                        ska köras.
********************************************************************************/
static inline void pwm_run_cycle(struct pwm* self)
{
   self->output_high(self->output);
   delay_us(self->input.pwm_on_us);
   self->output_low(self->output);
   delay_us(self->input.pwm_off_us);
   return;
}