/********************************************************************************
* eeprom.c: Innehåller funktionsdefinitioner för skrivning samt läsning till 
*           och från EEPROM-minnet.
********************************************************************************/
#include "eeprom.h"

/********************************************************************************
* eeprom_write_byte: Skriver en byte bestående av ett osignerat heltal till
*                    angiven adress i EEPROM-minnet. Vid lyckad skrivning
*                    returneras 0, annars returneras felkod 1.
*
*                    1. Om angiven adress överstiger högsta adressen i EEPROM-
*                       minnet sker ingen skrivning och felkod 1 returneras.
*
*                    2. Eventuell föregående skrivning avslutas innan den
*                       nya skrivningen påbörjas.
*
*                    3. Angiven adress samt datan som ska skrivas specificeras.
*
*                    4. Avbrott inaktiveras temporärt för att inte avbryta
*                       eventuell skrivning (EEPROM-skrivningen måste ske
*                       inom fyra klockcykler för att lyckas).
*
*                    5. Skrivningen genomförs.
*
*                    6. Avbrott återaktiveras.
*                       
*                    - address: Adressen i EEPROM-minnet som angiven data
*                               ska lagras på.
*                    - data   : Datan som ska skrivas.
********************************************************************************/
int eeprom_write_byte(const uint16_t address,
                      const uint8_t data)
{
   if (address > EEPROM_ADDRESS_MAX) return 1;
   while (EECR & (1 << EEPE));
   EEAR = address;
   EEDR = data;

   asm("CLI");
   EECR |= (1 << EEMPE);
   EECR |= (1 << EEPE);
   asm("SEI");
   return 0;
}

/********************************************************************************
* eeprom_write_word: Skriver två byte bestående av ett osignerat heltal till
*                    angiven samt efterföljande adress i EEPROM-minnet. Vid
*                    lyckad skrivning returneras 0, annars returneras felkod 1.
*
*                    1. Om angiven adress överstiger näst högsta adressen i
*                       EEPROM-minnet sker ingen skrivning och felkod 1 
*                       returneras.
*
*                    2. Det 16-bitars talet lagras som två separata byte.
*
*                    3. Respektive byte skrivs till EEPROM-minnet på 
*                       konsekutiva adresser.
*
*                    4. Returkod 0 returneras för att indikera lyckas skrivning.
*
*                    - address_low: Den lägre adressen i EEPROM-minnet som
*                                   angiven data ska lagras på.
*                    - data       : Datan som ska skrivas.
********************************************************************************/
int eeprom_write_word(const uint16_t address_low,
                      const uint16_t data)
{
   if (address_low > EEPROM_ADDRESS_MAX - 1) return 1;
   eeprom_write_byte(address_low, (uint8_t)(data));
   eeprom_write_byte(address_low + 1, (uint8_t)(data >> 8));
   return 0;
}

/********************************************************************************
* eeprom_read_byte: Läser en byte på angiven adress i EEPROM-minnet och
*                   returnerar detta som ett osignerat heltal. Vid misslyckad
*                   läsning returneras 0.
*
*                   1. Om angiven adress överstiger högsta adressen i EEPROM-
*                      minnet sker ingen läsning och 0 returneras.
*
*                   2. Eventuell föregående skrivning avslutas innan läsningen
*                      påbörjas.
*
*                   3. Angiven adress som läsning ska ske från specificeras.
*
*                   4. Läsningen genomförs från angiven adress.
*
*                   5. Innehållet returneras som ett 8-bitars osignerat heltal.
*
*                   - address: Adressen i EEPROM-minnet som ska läsas av.
********************************************************************************/
uint8_t eeprom_read_byte(const uint16_t address)
{
   if (address > EEPROM_ADDRESS_MAX) return 0;
   while (EECR & (1 << EEPE));
   EEAR = address;
   EECR |= (1 << EERE);
   return EEDR;
}

/********************************************************************************
* eeprom_read_word: Läser två byte på angiven samt efterföljande adress i
*                   EEPROM-minnet och returnerar detta som ett osignerat heltal.
*                   Vid misslyckad läsning returneras 0.
*
*                   1. Om angiven adress överstiger högsta adressen i EEPROM-
*                      minnet sker ingen läsning och 0 returneras.
*
*                   2. Respektive byte läses från EEPROM-minnet en i taget
*                      och lagras som ett ord (ett 16-bitars tal).
*
*                   3. Avläst 16-bitars tal returneras.
*
*                   - address_low: Den lägre adressen i EEPROM-minnet som
*                                  ska läsas av.
********************************************************************************/
uint16_t eeprom_read_word(const uint16_t address_low)
{
   if (address_low >= EEPROM_ADDRESS_MAX - 1) return 0;
   return eeprom_read_word(address_low) | (eeprom_read_word(address_low + 1) << 8);
}