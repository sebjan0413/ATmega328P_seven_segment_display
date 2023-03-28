/********************************************************************************
* display.h: Innehåller drivrutiner för två 7-segmentsdisplayer anslutna till
*            till PORTD0 - PORTD6 (pin 0 - 6), som kan visa två siffror i binär,
*            decimal eller hexadecimal form. Matningsspänningen för respektive
*            7-segmentsdisplay genereras från PORTD7 (pin 7) respektive
*            PORTC3 (pin A3), där låg signal medför tänd display, då displayerna
*            har gemensam katod.
********************************************************************************/
#include "display.h"
#include "eeprom.h"

/********************************************************************************
* Makrodefinitioner:
********************************************************************************/
#define DISPLAY1_CATHODE PORTD7 /* Katod för display 1. */
#define DISPLAY2_CATHODE PORTC3 /* Katod för display 1. */

#define DISPLAY1_ON  PORTD &= ~(1 << DISPLAY1_CATHODE) /* Tänder display 1. */
#define DISPLAY2_ON  PORTC &= ~(1 << DISPLAY2_CATHODE) /* Tänder display 2. */
#define DISPLAY1_OFF PORTD |= (1 << DISPLAY1_CATHODE)  /* Släcker display 1. */
#define DISPLAY2_OFF PORTC |= (1 << DISPLAY2_CATHODE)  /* Släcker display 1. */

#define OFF 0x00   /* Binärkod för släckning av 7-segmentsdisplay. */
#define ZERO 0x3F  /* Binärkod för utskrift av heltalet 0 på 7-segmentsdisplay. */
#define ONE 0x06   /* Binärkod för utskrift av heltalet 1 på 7-segmentsdisplay. */
#define TWO 0x5B   /* Binärkod för utskrift av heltalet 2 på 7-segmentsdisplay. */
#define THREE 0x4F /* Binärkod för utskrift av heltalet 3 på 7-segmentsdisplay. */
#define FOUR 0x66  /* Binärkod för utskrift av heltalet 4 på 7-segmentsdisplay. */
#define FIVE 0x6D  /* Binärkod för utskrift av heltalet 5 på 7-segmentsdisplay. */
#define SIX 0x7D   /* Binärkod för utskrift av heltalet 6 på 7-segmentsdisplay. */
#define SEVEN 0x07 /* Binärkod för utskrift av heltalet 7 på 7-segmentsdisplay. */
#define EIGHT 0x7F /* Binärkod för utskrift av heltalet 8 på 7-segmentsdisplay. */
#define NINE 0x6F  /* Binärkod för utskrift av heltalet 9 på 7-segmentsdisplay. */
#define A 0x77     /* Binärkod för utskrift av heltalet 0xA (10) på 7-segmentsdisplay. */
#define B 0x7C     /* Binärkod för utskrift av heltalet 0xB (11) på 7-segmentsdisplay. */
#define C 0x39     /* Binärkod för utskrift av heltalet 0xC (12) på 7-segmentsdisplay. */
#define D 0x5E     /* Binärkod för utskrift av heltalet 0xD (13) på 7-segmentsdisplay. */
#define E 0x79     /* Binärkod för utskrift av heltalet 0xE (14) på 7-segmentsdisplay. */
#define F 0x71     /* Binärkod för utskrift av heltalet 0xF (15) på 7-segmentsdisplay. */

#define EEPROM_NUMBER 100				// Här sparas talet på displayerna
#define EEPROM_OUTPUT_ENABLED 101	// Här sparas om displayerna är på (1 = på, 0 = av).
#define EEPROM_COUNT_ENABLED 102		// Här sparas om räkningen är på eller inte (1 = på, 0 = av).
#define EEPROM_COUNT_DIRECTION 103	// Här sparas i vilken riktning som räknaren går (1 = på, 0 = av).
#define EEPROM_INITIALIZED 104		// Här sparas om EEPROM har initierats eller ej (0 = sant, 0xFF = falskt).

/********************************************************************************
* display_digit: Enumeration för selektion av de olika displayerna.
********************************************************************************/
enum display_digit
{
	DISPLAY_DIGIT1, /* Display 1, som visar tiotal. */
	DISPLAY_DIGIT2  /* Display 2, som visar ental. */
};

/********************************************************************************
* Statiska funktioner:
********************************************************************************/
static inline void display_update_output(const uint8_t digit);
static inline uint8_t display_get_binary_code(const uint8_t digit);
static inline void check_eeprom_values(void);
static inline void reset_eeprom_values(void);

/********************************************************************************
* Statiska variabler:
*
*   - number : Talet som skrivs ut på displayerna.
*   - digit1 : Tiotalet som skrivs ut på display 1.
*   - digit2 : Entalet som skrivs ut på display 2.
*   - radix  : Talbas (default = 10, dvs. decimal form).
*   - max_val: Maxvärde för tal på 7-segmentsdisplayerna (beror på talbasen).
*
*   - count_direction: Indikerar räkningsriktning, där default är uppräkning.
*   - current_digit  : Indikerar vilken av aktuellt tals siffror som skrivs ut
*                      på aktiverad 7-segmentsdisplay, där default är tiotalet
*                      på display 1.
*
*   - timer_digit      : Timerkrets för att skifta displayer (Timer 1).
*   - timer_count_speed: Timerkrets för uppräkning av heltal (Timer 2).
********************************************************************************/
static uint8_t number = 0;
static uint8_t digit1 = 0;
static uint8_t digit2 = 0;
static uint8_t radix = 10;
static uint8_t max_val = 99;

static enum display_count_direction count_direction = DISPLAY_COUNT_DIRECTION_UP;
static enum display_digit current_digit = DISPLAY_DIGIT1;

static struct timer timer_digit;
static struct timer timer_count_speed;

/********************************************************************************
* display_init: Initierar hårdvara för 7-segmentsdisplayer.
********************************************************************************/
void display_init(void)
{
	DDRD = 0xFF;
	DDRC |= (1 << DISPLAY2_CATHODE);
	DISPLAY1_OFF;
	DISPLAY2_OFF;

	timer_init(&timer_digit, TIMER_SEL_1, 1); // Skiftar siffra en gång per ms.
	timer_init(&timer_count_speed, TIMER_SEL_2, 1000);
	
	check_eeprom_values(); // Vi kollar om det finns gamla värden sparade i EEPROM och läser i så fall in dem.
	
	return;
}
/********************************************************************************
* display_reset: Återställer 7-segmentsdisplayer till startläget.
********************************************************************************/
void display_reset(void)
{
	timer_reset(&timer_digit);
	timer_reset(&timer_count_speed);
	DISPLAY1_OFF;
	DISPLAY2_OFF;

	number = 0;
	digit1 = 0;
	digit2 = 0;
	radix = 10;
	max_val = 99;
	
	count_direction = DISPLAY_COUNT_DIRECTION_UP;
	current_digit = DISPLAY_DIGIT1;
	reset_eeprom_values();
	return;
}

/********************************************************************************
* display_output_enabled: Indikerar ifall 7-segmentsdisplayerna är påslagna.
*                         Vid påslagna displayer returneras true, annars false.
********************************************************************************/
bool display_output_enabled(void)
{
	eeprom_write_byte(EEPROM_OUTPUT_ENABLED, 1);
	return timer_interrupt_enabled(&timer_digit);
}

/********************************************************************************
* display_count_enabled: Indikerar ifall upp- eller nedräkning av tal på
*                        7-segmentsdisplayerna är aktiverat. Ifall uppräkning
*                        är aktiverat returneras true, annars false.
********************************************************************************/
bool display_count_enabled(void)
{
	eeprom_write_byte(EEPROM_COUNT_ENABLED, 1);
	return timer_interrupt_enabled(&timer_count_speed);
}

/********************************************************************************
* display_enable_output: Sätter på 7-segmentsdisplayer.
********************************************************************************/
void display_enable_output(void)
{
	timer_enable_interrupt(&timer_digit);
	// I EEPROM, sparas att displayerna är på:
	eeprom_write_byte(EEPROM_OUTPUT_ENABLED, 1);
	return;
}

/********************************************************************************
* display_disable_output: Stänger av 7-segmentsdisplayer.
********************************************************************************/
void display_disable_output(void)
{
	timer_reset(&timer_digit);
	DISPLAY1_OFF;
	DISPLAY2_OFF;
	// I EEPROM, sparas att displayerna är av:
	eeprom_write_byte(EEPROM_OUTPUT_ENABLED, 0);
	return;
}


/********************************************************************************
* display_toggle_output: Togglar aktivering av 7-segmentsdisplayer.
********************************************************************************/
void display_toggle_output(void)
{
	if (display_output_enabled())
	{
		display_disable_output();
	}
	else
	{
		display_enable_output();
	}
	return;
}

/********************************************************************************
* display_set_number: Sätter nytt heltal för utskrift på 7-segmentsdisplayer.
*                     Om angivet heltal överstiger maxvärdet som kan skrivas ut
*                     på två 7-segmentsdisplayer med aktuell talbas returneras
*                     felkod 1. Annars returneras heltalet 0 efter att heltalet
*                     på 7-segmentsdisplayerna har uppdaterats.
*
*                     - new_number: Nytt tal som ska skrivas ut på displayerna.
********************************************************************************/
int display_set_number(const uint8_t new_number)
{
	if (new_number <= max_val)
	{
		number = new_number;	
		digit1 = number / radix; 
		digit2 = number - (digit1 * radix);
		
		eeprom_write_byte(EEPROM_NUMBER, number);
		return 0;
	}
	else 
	{
		return 1;
	}
}

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
int display_set_radix(const uint8_t new_radix)
{
	if(new_radix == 2)
	{
		max_val = 0b11; // 3
	}
	else if(new_radix == 10)
	{
		max_val = 99;
	}
	else if(new_radix == 16)
	{
		max_val = 0xFF; // 255
	}
	else
	{
		return 1;
	}
	
	radix = new_radix;
	return 0;
}

/********************************************************************************
* display_toggle_digit: Skiftar aktiverad 7-segmentsdisplay för utskrift av
*                       tiotal och ental, vilket är nödvändigt, då displayerna
*                       delar på samma pinnar. Enbart en värdesiffra skrivs
*                       ut om möjligt, exempelvis 9 i stället för 09. Denna
*                       funktion bör anropas en gång per millisekund för att
*                       ett givet tvåsiffrigt tal ska upplevas skrivas ut
*                       kontinuerligt.
********************************************************************************/
void display_toggle_digit(void)
{
	timer_count(&timer_digit);
	
	if (timer_elapsed(&timer_digit))
	{
		current_digit = !current_digit;
		
		if (current_digit == DISPLAY_DIGIT1)
		{
			DISPLAY2_OFF;
			
			if (digit1 == 0) 
			{
				DISPLAY1_OFF;
			}
			else
			{
				display_update_output(digit1);
				DISPLAY1_ON;	
			}
		}
		else
		{
			DISPLAY1_OFF;
			display_update_output(digit2);
			DISPLAY2_ON;
		}
	}
	return;
}

/********************************************************************************
* display_count: Räknar upp eller ned tal på 7-segmentsdisplayer.
*
*					  1. Räknar upp löpt tid via timern timer_count_speed.
*					  2. När timern löpt ut sker upp/nedräkning
					  3. Vid uppräkning, inkrementera variabeln number upp  till
					     och med aktuellt maxvärde max_val, annars nollställ.
					  4. Vid nedräkning dekrementeras variabeln number till 0, 
						  därefter sätts den till max_val.
					  5. Uppdaterar värdet med hjälp av display_set_number.
********************************************************************************/
void display_count(void)
{
	timer_count(&timer_count_speed);
	
	if (timer_elapsed(&timer_count_speed))
	{
		if (count_direction == DISPLAY_COUNT_DIRECTION_UP)
		{
			if (number >= max_val) number = 0;
			else number++;
		}
		else
		{
			if(number == 0) number = max_val;
			else number--;
		}
		
		display_set_number(number);
	}

	return;
}

/********************************************************************************
* display_set_count_direction: Sätter ny uppräkningsriktning för tal som skrivs
*                              ut på 7-segmentsdisplayer.
*
*                              - new_direction: Ny uppräkningsriktning.
********************************************************************************/
void display_set_count_direction(const enum display_count_direction new_direction)
{
	count_direction = new_direction;
	eeprom_write_byte(EEPROM_COUNT_DIRECTION, (uint8_t)(new_direction));
	return;
}

/********************************************************************************
* display_toggle_count_direction: Togglar uppräkningsriktning för tal som skrivs
*                                 ut på 7-segmentsdisplayer.
********************************************************************************/
void display_toggle_count_direction(void)
{
	count_direction = !count_direction;
	// Här sparass uppräkningsriktningen
	eeprom_write_byte(EEPROM_COUNT_DIRECTION, (uint8_t)(count_direction));
	return;
}

/********************************************************************************
* display_set_count: Ställer in upp- eller nedräkning av tal som skrivs ut på
*                    7-segmentsdisplayerna med godtycklig uppräkningshastighet.
*
*                    - direction     : Uppräkningsriktning.
*                    - count_speed_ms: Uppräkningshastighet mätt i ms.
********************************************************************************/
void display_set_count(const enum display_count_direction direction, uint16_t count_speed_ms)
{
	count_direction = direction;
	timer_set_new_time(&timer_count_speed, count_speed_ms);
	return;
}

/********************************************************************************
* display_enable_count: Aktiverar upp- eller nedräkning av tal som skrivs ut på
*                       7-segmentsdisplayerna. Som default används en
*                       uppräkningshastighet på 1000 ms om inget annat angetts
*                       (via anrop av funktionen display_set_count).
********************************************************************************/
void display_enable_count(void)
{
	timer_enable_interrupt(&timer_count_speed);
	eeprom_write_byte(EEPROM_COUNT_ENABLED, 1);
	return;
}

/********************************************************************************
* display_disable_count: Inaktiverar upp- eller nedräkning av tal som skrivs ut
*                        på 7-segmentsdisplayerna.
********************************************************************************/
void display_disable_count(void)
{
	timer_reset(&timer_count_speed);
	eeprom_write_byte(EEPROM_COUNT_ENABLED, 0);
	return;
}

/********************************************************************************
* display_toggle_count: Togglar upp- eller nedräkning av tal som skrivs ut på
*                       7-segmentsdisplayerna.
********************************************************************************/
void display_toggle_count(void)
{
	if (display_count_enabled())
	{
		display_disable_count();
	}
	else
	{
		display_enable_count();
		
	}
	return;
}

/********************************************************************************
* display_update_output: Skriver ny siffra till aktiverad 7-segmentsdisplay.
********************************************************************************/
static inline void display_update_output(const uint8_t digit)
{
   PORTD &= (1 << DISPLAY1_CATHODE);
   PORTD |= display_get_binary_code(digit);
   return;
}

/********************************************************************************
* display_get_binary_code: Returnerar binärkod för angivet heltal 0 - 15 för
*                          utskrift på 7-segmentsdisplayer. Vid felaktigt
*                          angivet heltal (över 15) returneras binärkod för
*                          släckning av 7-segmentsdisplayer.
*
*                          - digit: Heltal vars binärkod ska returneras.
********************************************************************************/
static inline uint8_t display_get_binary_code(const uint8_t digit)
{
   if (digit == 0)       return ZERO;
   else if (digit == 1)  return ONE;
   else if (digit == 2)  return TWO;
   else if (digit == 3)  return THREE;
   else if (digit == 4)  return FOUR;
   else if (digit == 5)  return FIVE;
   else if (digit == 6)  return SIX;
   else if (digit == 7)  return SEVEN;
   else if (digit == 8)  return EIGHT;
   else if (digit == 9)  return NINE;
   else if (digit == 10) return A;
   else if (digit == 11) return B;
   else if (digit == 12) return C;
   else if (digit == 13) return D;
   else if (digit == 14) return E;
   else if (digit == 15) return F;
   else                  return OFF;
}

/********************************************************************************
* check_eeprom_values: Kollar om det finns några sparade värden i EEPROM.
*							  om det finns, läses dessa in och displayen sätts i samma 
*							  tillstånd som de var i senast programmet kördes.
*
*							  1. Vi kollar om EEPROM är initierat. Då ligger värdet 0
*								  på EEPROM_INITIALIZED, annars ligger startvärdet 0xFF
*							  2. Om EEPROM är initierat läses värdena in, annars sätts
*							     addresserna i starttillståndet.
*							  3. Först läses det senaste talet in och sparas i variabeln
*								  number. Tiotaklet och entalet sparas i variabler digit1
*								  och digit2 via anrop av display_set_number.
*							  4. Läs in uppräkningsriktningen och spara i variabeln 
*							     count_direction. Det inlästa talet typomvandlas till
*								  enum display_count_direction.
*							  5. OM displayerna var påslagna innan programmet avslutades 
*							     tänds de.
*							  6. Om uppräkningen var påslaget innan programmet avslutades
*							     så startas uppräkningen direkt.
*********************************************************************************/
static inline void check_eeprom_values(void)
{
	if (eeprom_read_byte(EEPROM_INITIALIZED) == 0)
	{
		display_set_number(eeprom_read_byte(EEPROM_NUMBER));
		count_direction = (enum display_count_direction)(eeprom_read_byte(EEPROM_COUNT_DIRECTION));
		
		if (eeprom_read_byte(EEPROM_OUTPUT_ENABLED) == 1)
		{
			display_enable_output();
		}
		if (eeprom_read_byte(EEPROM_COUNT_ENABLED) == 1)
		{
			display_enable_count();
		}
	}
	else
	{
		reset_eeprom_values();
	}
	return;
}

/********************************************************************************
* reset_eeprom_values: Skriver startvärden till EEPROM. 
*							  number = 0;
*							  output_enabled = false
*							  count_enabled = false
*							  count_direction = up;
*							  eeprom_initialized = 0 (Står för sant).
*********************************************************************************/
static inline void reset_eeprom_values(void)
{
	eeprom_write_byte(EEPROM_NUMBER, 0);
	eeprom_write_byte(EEPROM_OUTPUT_ENABLED, 0);
	eeprom_write_byte(EEPROM_COUNT_ENABLED, 0);
	eeprom_write_byte(EEPROM_COUNT_DIRECTION, 1);
	eeprom_write_byte(EEPROM_INITIALIZED, 0);
	return;
}