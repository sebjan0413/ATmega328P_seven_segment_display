/********************************************************************************
* adc.h: Innehåller funktionalitet för enkel implementering av AD-omvandling
*        för analoga signaler via strukten adc. AD-omvandling sker via inläsning
*        av signaler från analoga pinnar A0 - A5 på  Arduino Uno, vilket
*        motsvarar PORTC0 - PORTC5 på ATmega328P.
*
*        Analoga insignaler mellan 0 - 5 V AD-omvandlas till digitala
*        motsvarigheter mellan 0 - 1023. Duty cycle kan användas för
*        PWM-generering och beräknas enligt nedan:
*
*                       duty cycle = ADC_result / ADC_MAX,
*
*       där ADC_result är resultat avläst från AD-omvandlaren OCH ADC_MAX
*       utgör högsta möjliga avlästa värde, vilket är 1023.0.
********************************************************************************/
#ifndef ADC_H_
#define ADC_H_

/* Inkluderingsdirektiv: */
#include "misc.h"

/* Makrodefinitioner: */
#define ADC_MAX 1023.0 /* Högsta digitala värde vid AD-omvandling (motsvarar 5 V). */
#define VCC 5.0        /* 5 V matningsspänning. */

/********************************************************************************
* adc: Strukt för implementering av AD-omvandlare, som möjliggör avläsning
*      av insignaler från analoga pinnar samt beräkning av on- och off-tid för
*      PWM-generering (tiden som PWM-styrd utsignal ska vara hög respektive låg).
********************************************************************************/
struct adc
{
   uint8_t pin;         /* Analog pin som ska användas för avläsning. */
   uint16_t pwm_on_us;  /* On-tid för PWM-generering i mikrosekunder. */
   uint16_t pwm_off_us; /* Off-tid för PWM-generering i mikrosekunder. */
};

/********************************************************************************
* adc_init: Initierar analog pin för avläsning och AD-omvandling av insignaler,
*           som antingen kan anges som ett tal mellan 0 - 5 eller via konstanter
*           A0 - A5 (som motsvarar heltal 14 - 19).
*
*           - self: Pekare till analog pin som ska användas för AD-omvandling.
*           - pin : Analog pin som ska läsas för AD-omvandling.
********************************************************************************/
void adc_init(struct adc* self,
              const uint8_t pin);

/********************************************************************************
* adc_clear: Nollställer angivet adc-objekt.
*
*            - self: Pekare till objektet som ska nollställas.
********************************************************************************/
static inline void adc_clear(struct adc* self)
{
   self->pin = 0;
   self->pwm_on_us = 0;
   self->pwm_off_us = 0;
   return;
}

/********************************************************************************
* adc_read: Läser av en analog insignal och returnerar motsvarande digitala
*           motsvarighet mellan 0 - 1023.
*
*           - self: Pekare till analog pin vars insignal ska AD-omvandlas.
********************************************************************************/
uint16_t adc_read(const struct adc* self);

/********************************************************************************
* adc_duty_cycle: Läser av en analog insignal och returnerar motsvarande
*                 duty cycle som ett flyttal mellan 0 - 1.
*
*                 - self: Pekare till analog pin vars insignal ska AD-omvandlas.
********************************************************************************/
static inline double adc_duty_cycle(const struct adc* self)
{
   return adc_read(self) / ADC_MAX;
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
                        uint16_t pwm_period_us);

/********************************************************************************
* adc_get_input_voltage: Returnerar inspänningen på angiven analog pin genom att
*                        läsa av insignalen, omvandla till motsvarande digitala
*                        värde och sedan beräkna motsvarande inspänning.
*
*                        Värdet beräknas efter en matningsspänning på 5 V,
*                        vilket medför en analog insignal mellan 0 - 5 V. 
*                        Genom att avläsa duty cycle (mellan 0 - 1) så erhålls 
*                        motsvarande analoga inspänning Uin, som returneras.
*     
*                        - self: Pekare till analog pin som ska läsas av.
********************************************************************************/
static inline double adc_get_input_voltage(const struct adc* self)
{
   return adc_duty_cycle(self) * VCC;
}

/********************************************************************************
* adc_get_temperature: Returnerar aktuell rumstemperatur via avläsning av
*                      temperatursensor TMP36, ansluten till angiven pin.
*
*                      Temperaturen T beräknas via följande formel:
*                      T = 100 * Uin - 50,
*                      där Uin är aktuell analog inspänning avläst från 
*                      temperatursensor TMP36.
*
*                      - self: Pekare till analog pin som ska läsas av.
********************************************************************************/
static inline double adc_get_temperature(const struct adc* self)
{
   return 100 * adc_get_input_voltage(self) - 50;
}

#endif /* ADC_H_ */