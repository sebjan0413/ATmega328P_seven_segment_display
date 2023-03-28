/********************************************************************************
* tmp36.c: Innehåller definitioner av drivrutiner för temperatursensor TMP36.
********************************************************************************/
#include "tmp36.h"

/********************************************************************************
* tmp36_init: Initierar pin ansluten till temperatursensor TMP36 för mätning
*             samt utskrift av rumstemperaturen. Seriell överföring initieras
*             också med en baud rate (överföringshastighet) på 9600 kbps.
*
*             - self: Pekare till temperatursensorn som ska initieras.
*             - pin : Analog pin A0 - A5 som temperatursensorn är ansluten till.
********************************************************************************/
void tmp36_init(struct tmp36* self,
                const uint8_t pin)
{
   adc_init(&self->adc, pin);
   serial_init(9600);
   return;
}

/********************************************************************************
* tmp36_print_temperature: Skriver ut aktuell rumstemperatur avläst av
*                          temperatursensor TMP36.
*
*                          - self: Pekare till temperatursensor TMP36.
********************************************************************************/
void tmp36_print_temperature(const struct tmp36* self)
{
   serial_print_string("Temperature: ");
   serial_print_double(tmp36_get_temperature(self));
   serial_print_string(" degrees Celcius.\n");
   return;
}

/********************************************************************************
* tmp36_print_voltage: Skriver ut aktuell inspänning avläst av temperatursensor
*                      TMP36 mätt i V.
*
*                      - self: Pekare till temperatursensor TMP36.
********************************************************************************/
void tmp36_print_voltage(const struct tmp36* self)
{
   serial_print_string("Voltage: ");
   serial_print_double(tmp36_get_input_voltage(self));
   serial_print_string(" V\n.");
   return;
}