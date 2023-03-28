/********************************************************************************
* display.h: Inneh�ller drivrutiner f�r tv� 7-segmentsdisplayer anslutna till
*            till PORTD0 - PORTD6 (pin 0 - 6), som kan visa tv� siffror i bin�r,
*            decimal eller hexadecimal form. Matningssp�nningen f�r respektive
*            7-segmentsdisplay genereras fr�n PORTD7 (pin 7) respektive
*            PORTC3 (pin A3), d�r l�g signal medf�r t�nd display, d� displayerna
*            har gemensam katod.
*
*            N�r displayerna �r p� skiftas aktiverad display en g�ng per
*            millisekund via timerkrets Timer 1. Vid anv�ndning av
*            7-segmentsdisplayer, anropa funktionen display_toggle_digit
*            i avbrottsrutinen f�r Timer 1 i CTC Mode s�som visas nedan:
*
*            ISR (TIMER1_COMPA_vect)
*            {
*               display_toggle_digit();
*               return;
*            }
*
*            Upp- eller nedr�kning med godtycklig hastighet kan aktiveras via
*            anrop av funktionen display_set_count, d�r timerkrets Timer 2
*            anv�nds f�r att generera uppr�kningshastigheten. Som exempel,
*            nedanst�ende funktionsanrop medf�r uppr�kning av
*            7-segmentsdisplayerna var 100:e ms:
*
*            display_set_count(DISPLAY_COUNT_DIRECTION_UP, 100);
*            display_enable_count();
*
*            Det �r inte n�dv�ndigt att ange uppr�kningsriktning samt 
*            hastighet innan aktivering via anrop av funktionen 
*            display_set_count. Som default anv�nds uppr�kning med 
*            en hastighet p� 1000 ms som default.
*
*            Vid upp- eller nedr�kning av talet p� 7-segmentsdisplayerna,
*            anropa funktionen display_count i avbrottsrutinen f�r Timer 2
*            i Normal Mode s�som visas nedan:
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
* display_count_direction: Enumeration f�r val av uppr�kningsriktning p�
*                          7-segmentsdisplayer.
********************************************************************************/
enum display_count_direction
{
   DISPLAY_COUNT_DIRECTION_UP		= 1,  /* Uppr�kning upp�t. */
   DISPLAY_COUNT_DIRECTION_DOWN	= 0 /* Uppr�kning ned�t. */
};

/********************************************************************************
* display_init: Initierar h�rdvara f�r 7-segmentsdisplayer.
********************************************************************************/
void display_init(void);

/********************************************************************************
* display_reset: �terst�ller 7-segmentsdisplayer till startl�get.
********************************************************************************/
void display_reset(void);

/********************************************************************************
* display_output_enabled: Indikerar ifall 7-segmentsdisplayerna �r p�slagna.
*                         Vid p�slagna displayer returneras true, annars false.
********************************************************************************/
bool display_output_enabled(void);

/********************************************************************************
* display_count_enabled: Indikerar ifall upp- eller nedr�kning av tal p�
*                        7-segmentsdisplayerna �r aktiverat. Ifall uppr�kning
*                        �r aktiverat returneras true, annars false.
********************************************************************************/
bool display_count_enabled(void);

/********************************************************************************
* display_enable_output: S�tter p� 7-segmentsdisplayer.
********************************************************************************/
void display_enable_output(void);

/********************************************************************************
* display_disable_output: St�nger av 7-segmentsdisplayer.
********************************************************************************/
void display_disable_output(void);

/********************************************************************************
* display_toggle_output: Togglar aktivering av 7-segmentsdisplayer.
********************************************************************************/
void display_toggle_output(void);

/********************************************************************************
* display_set_number: S�tter nytt heltal f�r utskrift p� 7-segmentsdisplayer.
*                     Om angivet heltal �verstiger maxv�rdet som kan skrivas ut
*                     p� tv� 7-segmentsdisplayer med aktuell talbas returneras
*                     felkod 1. Annars returneras heltalet 0 efter att heltalet
*                     p� 7-segmentsdisplayerna har uppdaterats.
*
*                     - new_number: Nytt tal som ska skrivas ut p� displayerna.
********************************************************************************/
int display_set_number(const uint8_t new_number);

/********************************************************************************
* display_set_radix: S�tter ny talbas till 2, 10 eller 16 utskrift av tal p�
*                    7-segmentsdisplayer. D�rmed kan tal skrivas ut bin�rt
*                    (00 - 11), decimalt (00 - 99) eller hexadecimalt (00 - FF).
*                    Vid felaktigt angiven talbas returneras felkod 1. Annars
*                    returneras heltalet 0 efter att anv�nd talbas har
*                    uppdaterats.
*
*                    - new_radix: Ny talbas f�r tal som skrivs ut p� displayerna.
********************************************************************************/
int display_set_radix(const uint8_t new_radix);

/********************************************************************************
* display_toggle_digit: Skiftar aktiverad 7-segmentsdisplay f�r utskrift av
*                       tiotal och ental, vilket �r n�dv�ndigt, d� displayerna
*                       delar p� samma pinnar. Enbart en v�rdesiffra skrivs
*                       ut om m�jligt, exempelvis 9 i st�llet f�r 09. Denna 
*                       funktion b�r anropas en g�ng per millisekund f�r att 
*                       ett givet tv�siffrigt tal ska upplevas skrivas ut 
*                       kontinuerligt.
********************************************************************************/
void display_toggle_digit(void);

/********************************************************************************
* display_count: R�knar upp eller ned tal p� 7-segmentsdisplayer.
********************************************************************************/
void display_count(void);

/********************************************************************************
* display_set_count_direction: S�tter ny uppr�kningsriktning f�r tal som skrivs
*                              ut p� 7-segmentsdisplayer.
*
*                              - new_direction: Ny uppr�kningsriktning.
********************************************************************************/
void display_set_count_direction(const enum display_count_direction new_direction);

/********************************************************************************
* display_toggle_count_direction: Togglar uppr�kningsriktning f�r tal som skrivs
*                                 ut p� 7-segmentsdisplayer.
********************************************************************************/
void display_toggle_count_direction(void);

/********************************************************************************
* display_set_count: St�ller in upp- eller nedr�kning av tal som skrivs ut p�
*                    7-segmentsdisplayerna med godtycklig uppr�kningshastighet.
*
*                    - direction     : Uppr�kningsriktning.
*                    - count_speed_ms: Uppr�kningshastighet m�tt i ms.
********************************************************************************/
void display_set_count(const enum display_count_direction direction, uint16_t count_speed_ms);

/********************************************************************************
* display_enable_count: Aktiverar upp- eller nedr�kning av tal som skrivs ut p�
*                       7-segmentsdisplayerna. Som default anv�nds en
*                       uppr�kningshastighet p� 1000 ms om inget annat angetts
*                       (via anrop av funktionen display_set_count).
********************************************************************************/
void display_enable_count(void);

/********************************************************************************
* display_disable_count: Inaktiverar upp- eller nedr�kning av tal som skrivs ut
*                        p� 7-segmentsdisplayerna.
********************************************************************************/
void display_disable_count(void);

/********************************************************************************
* display_toggle_count: Togglar upp- eller nedr�kning av tal som skrivs ut p�
*                       7-segmentsdisplayerna.
********************************************************************************/
void display_toggle_count(void);

#endif /* DISPLAY_H_ */