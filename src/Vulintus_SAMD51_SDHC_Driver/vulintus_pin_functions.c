#include "vulintus_pin_functions.h"

/** Master clock frequency (also Fcpu frequency) */
#define VARIANT_MCK		(48000000ul)

const PinDescription sdhc0_pin_description [] =
{
    { PORTA, 8 },       //SD_CMD      (0)  PIO_COM_02
    { PORTB, 11 },      //SD_CLK      (1)  PIO_COM_03
    { PORTA, 9 },       //SD_DAT0     (2)  PIO_COM_04
    { PORTA, 10 },      //SD_DAT1     (3)  PIO_COM_05
    { PORTA, 11 },      //SD_DAT2     (4)  PIO_COM_06
    { PORTB, 10 }       //SD_DAT3     (5)  PIO_COM_07
};

const PinDescription sdhc1_pin_description [] =
{
    { PORTA, 8 },       //SD_CMD      (0)  PIO_COM_02
    { PORTB, 11 },      //SD_CLK      (1)  PIO_COM_03
    { PORTA, 9 },       //SD_DAT0     (2)  PIO_COM_04
    { PORTA, 10 },      //SD_DAT1     (3)  PIO_COM_05
    { PORTA, 11 },      //SD_DAT2     (4)  PIO_COM_06
    { PORTB, 10 }       //SD_DAT3     (5)  PIO_COM_07
};

PinDescription *g_APinDescription;

static volatile uint32_t _ulTickCount=0 ;

void pin_mode ( uint32_t ulPin, uint32_t ulMode )
{
    EPortType port = g_APinDescription[ulPin].ulPort;
    uint32_t pin = g_APinDescription[ulPin].ulPin;
    uint32_t pinMask = (1ul << pin);

    // Set pin mode according to chapter '22.6.3 I/O Pin Configuration'
    switch ( ulMode )
    {
        case 0: //INPUT
            // Set pin to input mode
            PORT->Group[port].PINCFG[pin].reg=(uint8_t)(PORT_PINCFG_INEN) ;
            PORT->Group[port].DIRCLR.reg = pinMask ;
            break ;

        case 2: //INPUT_PULLUP
            // Set pin to input mode with pull-up resistor enabled
            PORT->Group[port].PINCFG[pin].reg=(uint8_t)(PORT_PINCFG_INEN|PORT_PINCFG_PULLEN) ;
            PORT->Group[port].DIRCLR.reg = pinMask ;

            // Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.7 Data Output Value Set')
            PORT->Group[port].OUTSET.reg = pinMask ;
            break ;

        case 3: //INPUT_PULLDOWN
            // Set pin to input mode with pull-down resistor enabled
            PORT->Group[port].PINCFG[pin].reg=(uint8_t)(PORT_PINCFG_INEN|PORT_PINCFG_PULLEN) ;
            PORT->Group[port].DIRCLR.reg = pinMask ;

            // Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.6 Data Output Value Clear')
            PORT->Group[port].OUTCLR.reg = pinMask ;
            break ;

        case 1: //OUTPUT
            // enable input, to support reading back values, with pullups disabled
            PORT->Group[port].PINCFG[pin].reg=(uint8_t)(PORT_PINCFG_INEN) ;

            // Set pin to output mode
            PORT->Group[port].DIRSET.reg = pinMask ;
            break ;

        default:
            // do nothing
            break ;
    }
}

void digital_write ( uint32_t ulPin, uint32_t ulVal )
{
    EPortType port = g_APinDescription[ulPin].ulPort;
    uint32_t pin = g_APinDescription[ulPin].ulPin;
    uint32_t pinMask = (1ul << pin);

    if ( (PORT->Group[port].DIRSET.reg & pinMask) == 0 ) 
    {
        // the pin is not an output, disable pull-up if val is LOW, otherwise enable pull-up
        PORT->Group[port].PINCFG[pin].bit.PULLEN = ((ulVal == 0) ? 0 : 1) ;
    }

    switch (ulVal)
    {
    case 0:
        PORT->Group[port].OUTCLR.reg = pinMask;
        break;
    default:
        PORT->Group[port].OUTSET.reg = pinMask;
        break;
    }

    return;
}

int digital_read ( uint32_t ulPin )
{
    if ( (PORT->Group[g_APinDescription[ulPin].ulPort].IN.reg & (1ul << g_APinDescription[ulPin].ulPin)) != 0 )
    {
        return 1;
    }

    return 0;
}

void set_pin_to_pio_sdhc (int ulPin)
{
    if ( g_APinDescription[ulPin].ulPin & 1 ) // is pin odd?
    {
        uint32_t temp;

        // Get whole current setup for both odd and even pins and remove odd one
        temp = (PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg) & PORT_PMUX_PMUXE( 0xF ) ;
        // Set new muxing
        PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg = temp|PORT_PMUX_PMUXO( 8 ) ;
        // Enable port mux
        PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg |= PORT_PINCFG_PMUXEN | PORT_PINCFG_DRVSTR;
    }
    else // even pin
    {
        uint32_t temp;

        temp = (PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg) & PORT_PMUX_PMUXO( 0xF ) ;
        PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg = temp|PORT_PMUX_PMUXE( 8 ) ;
        PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg |= PORT_PINCFG_PMUXEN | PORT_PINCFG_DRVSTR ; // Enable port mux
    }    
}

// Interrupt-compatible version of micros
// Theory: repeatedly take readings of SysTick counter, millis counter and SysTick interrupt pending flag.
// When it appears that millis counter and pending is stable and SysTick hasn't rolled over, use these
// values to calculate micros. If there is a pending SysTick, add one to the millis counter in the calculation.
unsigned long vulintus_micros( void )
{
    uint32_t ticks, ticks2;
    uint32_t pend, pend2;
    uint32_t count, count2;

    ticks2  = SysTick->VAL;
    pend2   = !!(SCB->ICSR & SCB_ICSR_PENDSTSET_Msk)  ;
    count2  = _ulTickCount ;

    do
    {
        ticks=ticks2;
        pend=pend2;
        count=count2;
        ticks2  = SysTick->VAL;
        pend2   = !!(SCB->ICSR & SCB_ICSR_PENDSTSET_Msk)  ;
        count2  = _ulTickCount ;
    } while ((pend != pend2) || (count != count2) || (ticks < ticks2));

    return ((count+pend) * 1000) + (((SysTick->LOAD  - ticks)*(1048576/(VARIANT_MCK/1000000)))>>20) ;
    // this is an optimization to turn a runtime division into two compile-time divisions and
    // a runtime multiplication and shift, saving a few cycles
}

void vulintus_delay(unsigned long ms)
{
    if (ms == 0)
    {
        return;
    }

    uint32_t start = vulintus_micros();

    while (ms > 0)
    {
        //yield();
        while (ms > 0 && (vulintus_micros() - start) >= 1000)
        {
            ms--;
            start += 1000;
        }
    }
}

void SysTick_DefaultHandler(void)
{
    // Increment tick count each ms
    _ulTickCount++;
}

