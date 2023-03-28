/********************************************************************************
* wdt.h: Innehåller drivrutiner för mikrokontrollerns Watchdog-timer, både i 
*        System Reset Mode, vilket innebär att mikrodatorn återställs vid 
*        timeout, samt i Interrupt Mode, vilket innebär avbrott vid timeout.
*        Det är också möjligt att kombinera tidigare nämnda modes för att
*        avbrott sker, följt av systemåterställning.
*
*        Avbrottsvektorn för timeout-avbrott är WDT_vect.
********************************************************************************/
#ifndef WDT_H_
#define WDT_H_

/* Inkluderingsdirektiv: */
#include "misc.h"

/********************************************************************************
* wdt_timeout: Enumeration för val av timeout för mikrodatorns Watchdog-timer.
********************************************************************************/
enum wdt_timeout
{
   WDT_TIMEOUT_16_MS   = 0,                                       /* 16 ms. */
   WDT_TIMEOUT_32_MS   = (1 << WDP0),                             /* 32 ms. */
   WDT_TIMEOUT_64_MS   = (1 << WDP1),                             /* 64 ms. */
   WDT_TIMEOUT_128_MS  = (1 << WDP1) | (1 << WDP0),               /* 128 ms. */
   WDT_TIMEOUT_256_MS  = (1 << WDP2),                             /* 256 ms. */
   WDT_TIMEOUT_512_MS  = (1 << WDP2) | (1 << WDP0),               /* 512 ms. */
   WDT_TIMEOUT_1024_MS = (1 << WDP2) | (1 << WDP1),               /* 1024 ms. */
   WDT_TIMEOUT_2048_MS = (1 << WDP2) | (1 << WDP1) | (1 << WDP0), /* 2048 ms. */
   WDT_TIMEOUT_4096_MS = (1 << WDP3),                             /* 4096 ms. */
   WDT_TIMEOUT_8192_MS = (1 << WDP3) | (1 << WDP0)                /* 8192 ms. */
};

/********************************************************************************
* wdt_reset: Återställer Watchdog-timern, vilket måste ske kontinuerligt innan
*            timern löper ut för att undvika systemåterställning eller avbrott.
********************************************************************************/
static inline void wdt_reset(void)
{
   asm("CLI");
   asm("WDR");
   MCUSR &= ~(1 << WDRF);
   asm("SEI");
   return;
}

/********************************************************************************
* wdt_init: Initierar Watchdog-timern med angiven timeout mätt i millisekunder.
*
*           - timeout_ms: Timeout mätt i millisekunder.
********************************************************************************/
static inline void wdt_init(const enum wdt_timeout timeout_ms)
{
   asm("CLI");
   WDTCSR = (1 << WDE) | (1 << WDCE);
   WDTCSR = (1 << WDE) | (uint8_t)(timeout_ms);
   asm("SEI");
   WDTCSR &= ~(1 << WDE);
   return;
}

/********************************************************************************
* wdt_clear: Nollställer Watchdog-timern.
********************************************************************************/
static inline void wdt_clear(void)
{
   wdt_reset();
   WDTCSR = 0x00;
   return;
}

/********************************************************************************
* wdt_enable_system_reset: Aktiverar Watchdog-timern i System Reset Mode,
*                          vilket innebär att systemet återställs ifall
*                          Watchdog-timern löper ut.
********************************************************************************/
static inline void wdt_enable_system_reset(void)
{
   wdt_reset();
   WDTCSR |= (1 << WDE);
   return;
}

/********************************************************************************
* wdt_disable_system_reset: Inaktiverar Watchdog-timern i System Reset Mode,
*                           vilket innebär att systemet inte återställs ifall
*                           Watchdog-timern löper ut.
********************************************************************************/
static inline void wdt_disable_system_reset(void)
{
   wdt_reset();
   WDTCSR &= ~(1 << WDE);
   return;
}

/********************************************************************************
* wdt_enable_interrupt: Aktiverar Watchdog-timern i Interrupt Mode, vilket
*                       innebär att ett avbrott med avbrottsvektor WDT_vect
*                       äger rum ifall Watchdog-timern löper ut.
********************************************************************************/
static inline void wdt_enable_interrupt(void)
{
   wdt_reset();
   WDTCSR |= (1 << WDIE);
   return;
}

/********************************************************************************
* wdt_disable_interrupt: Inaktiverar Watchdog-timern i Interrupt Mode, vilket
*                        innebär att avbrott inte äger rum ifall Watchdog-timern
*                        löper ut.
********************************************************************************/
static inline void wdt_disable_interrupt(void)
{
   wdt_reset();
   WDTCSR &= ~(1 << WDIE);
   return;
}

#endif /* WDT_H_ */