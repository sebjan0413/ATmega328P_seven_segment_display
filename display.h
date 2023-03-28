/********************************************************************************
* display.h: Innehåller drivrutiner för två 7-segmentsdisplayer anslutna till
*            till PORTD0 - PORTD6 (pin 0 - 6), som kan visa två siffror i binär,
*            decimal eller hexadecimal form. Matningsspänningen för respektive
*            7-segmentsdisplay genereras från PORTD7 (pin 7) respektive
*            PORTC3 (pin A3), där låg signal medför tänd display, då displayerna
*            har gemensam katod.
*
*            När displayerna är på skiftas aktiverad display en gång per
*            millisekund via timerkrets Timer 1. Vid användning av
*            7-segmentsdisplayer, anropa funktionen display_toggle_digit
*            i avbrottsrutinen för Timer 1 i CTC Mode såsom visas nedan:
*
*            ISR (TIMER1_COMPA_vect)
*            {
*               display_toggle_digit();
*               return;
*            }
*
*            Upp- eller nedräkning med godtycklig hastighet kan aktiveras via
*            anrop av funktionen display_set_count, där timerkrets Timer 2
*            används för att generera uppräkningshastigheten. Som exempel,
*            nedanstående funktionsanrop medför uppräkning av
*            7-segmentsdisplayerna var 100:e ms:
*
*            display_set_count(DISPLAY_COUNT_DIRECTION_UP, 100);
*            display_enable_count();
*
*            Det är inte nödvändigt att ange uppräkningsriktning samt 
*            hastighet innan aktivering via anrop av funktionen 
*            display_set_count. Som default används uppräkning med 
*            en hastighet på 1000 ms som default.
*
*            Vid upp- eller nedräkning av talet på 7-segmentsdisplayerna,
*            anropa funktionen display_count i avbrottsrutinen för Timer 2
*            i Normal Mode såsom visas nedan:
*
*            ISR (TIMER2_OVF_vect)
*            {
*               display_count();
*               return;
*            }
*
********************************************************************************/
#ifndef DISPLAY_H_
#define DISPLAY_H_

/********************************************************************************
* Inkluderingsdirektiv:
********************************************************************************/
#include "misc.h"
#include "timer.h"

/********************************************************************************
* display_count_direction: Enumeration för val av uppräkningsriktning på
*                          7-segmentsdisplayer.
********************************************************************************/
enum display_count_direction
{
   DISPLAY_COUNT_DIRECTION_UP		= 1,  /* Uppräkning uppåt. */
   DISPLAY_COUNT_DIRECTION_DOWN	= 0 /* Uppräkning nedåt. */
};

/********************************************************************************
* display_init: Initierar hårdvara för 7-segmentsdisplayer.
********************************************************************************/
void display_init(void);

/********************************************************************************
* display_reset: Återställer 7-segmentsdisplayer till startläget.
********************************************************************************/
void display_reset(void);

/********************************************************************************
* display_output_enabled: Indikerar ifall 7-segmentsdisplayerna är påslagna.
*                         Vid påslagna displayer returneras true, annars false.
********************************************************************************/
bool display_output_enabled(void);

/********************************************************************************
* display_count_enabled: Indikerar ifall upp- eller nedräkning av tal på
*                        7-segmentsdisplayerna är aktiverat. Ifall uppräkning
*                        är aktiverat returneras true, annars false.
********************************************************************************/
bool display_count_enabled(void);

/********************************************************************************
* display_enable_output: Sätter på 7-segmentsdisplayer.
********************************************************************************/
void display_enable_output(void);

/********************************************************************************
* display_disable_output: Stänger av 7-segmentsdisplayer.
********************************************************************************/
void display_disable_output(void);

/********************************************************************************
* display_toggle_output: Togglar aktivering av 7-segmentsdisplayer.
********************************************************************************/
void display_toggle_output(void);

/********************************************************************************
* display_set_number: Sätter nytt heltal för utskrift på 7-segmentsdisplayer.
*                     Om angivet heltal överstiger maxvärdet som kan skrivas ut
*                     på två 7-segmentsdisplayer med aktuell talbas returneras
*                     felkod 1. Annars returneras heltalet 0 efter att heltalet
*                     på 7-segmentsdisplayerna har uppdaterats.
*
*                     - new_number: Nytt tal som ska skrivas ut på displayerna.
********************************************************************************/
int display_set_number(const uint8_t new_number);

/********************************************************************************
* display_set_radix: Sätter ny talbas till 2, 10 eller 16 utskrift av tal på
*                    7-segmentsdisplayer. Därmed kan tal skrivas ut binärt
*                    (00 - 11), decimalt (00 - 99) eller hexadecimalt (00 - FF).
*                    Vid felaktigt angiven talbas returneras felkod 1. Annars
*                    returneras heltalet 0 efter att använd talbas har
*                    uppdaterats.
*
*                    - new_radix: Ny talbas för tal som skrivs ut på displayerna.
********************************************************************************/
int display_set_radix(const uint8_t new_radix);

/********************************************************************************
* display_toggle_digit: Skiftar aktiverad 7-segmentsdisplay för utskrift av
*                       tiotal och ental, vilket är nödvändigt, då displayerna
*                       delar på samma pinnar. Enbart en värdesiffra skrivs
*                       ut om möjligt, exempelvis 9 i stället för 09. Denna 
*                       funktion bör anropas en gång per millisekund för att 
*                       ett givet tvåsiffrigt tal ska upplevas skrivas ut 
*                       kontinuerligt.
********************************************************************************/
void display_toggle_digit(void);

/********************************************************************************
* display_count: Räknar upp eller ned tal på 7-segmentsdisplayer.
********************************************************************************/
void display_count(void);

/********************************************************************************
* display_set_count_direction: Sätter ny uppräkningsriktning för tal som skrivs
*                              ut på 7-segmentsdisplayer.
*
*                              - new_direction: Ny uppräkningsriktning.
********************************************************************************/
void display_set_count_direction(const enum display_count_direction new_direction);

/********************************************************************************
* display_toggle_count_direction: Togglar uppräkningsriktning för tal som skrivs
*                                 ut på 7-segmentsdisplayer.
********************************************************************************/
void display_toggle_count_direction(void);

/********************************************************************************
* display_set_count: Ställer in upp- eller nedräkning av tal som skrivs ut på
*                    7-segmentsdisplayerna med godtycklig uppräkningshastighet.
*
*                    - direction     : Uppräkningsriktning.
*                    - count_speed_ms: Uppräkningshastighet mätt i ms.
********************************************************************************/
void display_set_count(const enum display_count_direction direction, uint16_t count_speed_ms);

/********************************************************************************
* display_enable_count: Aktiverar upp- eller nedräkning av tal som skrivs ut på
*                       7-segmentsdisplayerna. Som default används en
*                       uppräkningshastighet på 1000 ms om inget annat angetts
*                       (via anrop av funktionen display_set_count).
********************************************************************************/
void display_enable_count(void);

/********************************************************************************
* display_disable_count: Inaktiverar upp- eller nedräkning av tal som skrivs ut
*                        på 7-segmentsdisplayerna.
********************************************************************************/
void display_disable_count(void);

/********************************************************************************
* display_toggle_count: Togglar upp- eller nedräkning av tal som skrivs ut på
*                       7-segmentsdisplayerna.
********************************************************************************/
void display_toggle_count(void);

#endif /* DISPLAY_H_ */