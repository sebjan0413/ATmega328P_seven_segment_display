/********************************************************************************
* adc.c: Innehåller funktionsdefinitioner för implementering av AD-omvandling
*        för analoga signaler via strukten adc.
********************************************************************************/
#include "adc.h"

/********************************************************************************
* adc_init: Initierar analog pin för avläsning och AD-omvandling av insignaler,
*           som antingen kan anges som ett tal mellan 0 - 5 eller via konstanter
*           A0 - A5 (som motsvarar heltal 14 - 19).
*
*           - self: Pekare till analog pin som ska användas för AD-omvandling.
*           - pin : Analog pin som ska läsas för AD-omvandling.
********************************************************************************/
void adc_init(struct adc* self,
              const uint8_t pin)
{
   if (pin >= 0 && pin <= 5)
   {
      self->pin = pin;
   }
   else if (pin >= 14 && pin <= 19)
   {
      self->pin = pin - 14;
   }

   self->pwm_on_us = 0;
   self->pwm_off_us = 0;

   (void)adc_read(self);
   return;
}

/********************************************************************************
* adc_read: Läser av en analog insignal och returnerar motsvarande digitala
*           motsvarighet mellan 0 - 1023.
*
*           - self: Pekare till analog pin som ska läsas av.
********************************************************************************/
uint16_t adc_read(const struct adc* self)
{
   ADMUX = (1 << REFS0) | self->pin;
   ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
   while ((ADCSRA & (1 << ADIF)) == 0);
   ADCSRA = (1 << ADIF);
   return ADC;
}

/********************************************************************************
* adc_get_pwm_values: Läser av en analog insignal och beräknar on- och off-tid
*                     för PWM-generering, avrundat till närmaste heltal.
*
*                     - self         : Pekare till analog pin som ska läsas av.
*                     - pwm_period_us: PWM-perioden (on-tid + off-tid) mätt i
*                                      mikrosekunder (default = 10 000 us).
********************************************************************************/
void adc_get_pwm_values(struct adc* self,
                        uint16_t pwm_period_us)
{
   if (!pwm_period_us) pwm_period_us = 10000;
   self->pwm_on_us = (uint16_t)(adc_duty_cycle(self) * pwm_period_us + 0.5);
   self->pwm_off_us = pwm_period_us - self->pwm_on_us;
   return;
}