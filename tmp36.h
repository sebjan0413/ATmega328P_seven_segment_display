/********************************************************************************
* tmp36.h: Innehåller drivrutiner för temperatursensor TMP36, som används för
*          mätning samt utskrift av rumstemperaturen. Den analoga insignalen
*          från TMP36 AD-omvandlas till en digital motsvarighet, som används
*          för att beräkna aktuell temperatur. Utskrift sker till en seriell
*          terminal via USART.
********************************************************************************/
#ifndef TMP36_H_
#define TMP36_H_

/* Inkluderingsdirektiv: */
#include "adc.h"
#include "serial.h"

/* Makrodefinitioner: */
#define VCC 5.0 /* 5 V matningsspänning. */

/********************************************************************************
* tmp36: Strukt för implementering av temperatursensor TMP36, som används för
*        mätning samt utskrift av rumstemperaturen. Vid avläsning AD-omvandlas
*        den analoga insignalen till en digital motsvarighet. Utefter detta
*        värde beräknas den analoga inspänningen Uin via nedanstående formel:
*
*        Uin = ADC_result / ADC_MAX * Vcc,
*
*        där ADC_result är den AD-omvandlade insignalen (0 - 1023),
*        ADC_MAX är högsta möjliga digitala signal (1023) och Vcc är 
*        mikrodatorns matningsspänning (5 V).
*
*        Temperaturen T beräknas utefter detta värde via nedanstående formel:
*
*        T = 100 * Uin - 50,
*
*        där Uin utgör analog inspänning avläst från temperatursensor TMP36.
********************************************************************************/
struct tmp36
{
   struct adc adc; /* AD-omvandlare, omvandlar analog insignal från TMP36. */
};

/********************************************************************************
* tmp36_init: Initierar pin ansluten till temperatursensor TMP36 för mätning
*             samt utskrift av rumstemperaturen. Seriell överföring initieras
*             också med en baud rate (överföringshastighet) på 9600 kbps.
*
*             - self: Pekare till temperatursensorn som ska initieras.
*             - pin : Analog pin A0 - A5 som temperatursensorn är ansluten till.
********************************************************************************/
void tmp36_init(struct tmp36* self, 
                const uint8_t pin);

/********************************************************************************
* adc_get_input_voltage: Returnerar inspänningen från angiven tempsensor genom
*                        att läsa av insignalen, omvandla till motsvarande
*                        digitala värde och sedan beräkna motsvarande inspänning.
*                        Värdet beräknas efter en matningsspänning på 5 V.
*
*                        - self: Pekare till temperatursensor TMP36.
********************************************************************************/
static inline double tmp36_get_input_voltage(const struct tmp36* self)
{
   return adc_duty_cycle(&self->adc) * VCC;
}

/********************************************************************************
* tmp36_get_temperature: Returnerar aktuell rumstemperatur via avläsning av
*                        angiven temperatursensor TMP36.
*
*                        - self: Pekare till temperatursensor TMP36.
********************************************************************************/
static inline double tmp36_get_temperature(const struct tmp36* self)
{
   return 100 * tmp36_get_input_voltage(self) - 50;
}

/********************************************************************************
* tmp36_print_temperature: Skriver ut aktuell rumstemperatur avläst av
*                          temperatursensor TMP36.
*
*                          - self: Pekare till temperatursensor TMP36.
********************************************************************************/
void tmp36_print_temperature(const struct tmp36* self);

/********************************************************************************
* tmp36_print_voltage: Skriver ut aktuell inspänning avläst av temperatursensor
*                      TMP36 mätt i V.
*
*                      - self: Pekare till temperatursensor TMP36.
********************************************************************************/
void tmp36_print_voltage(const struct tmp36* self);

#endif /* TMP36_H_ */