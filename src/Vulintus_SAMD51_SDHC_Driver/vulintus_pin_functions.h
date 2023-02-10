#ifndef __VULINTUS_PIN_FUNCTIONS_H
#define __VULINTUS_PIN_FUNCTIONS_H

#include <stdint.h>
#include "sam.h"

typedef enum _EPortType
{
  NOT_A_PORT=-1,
  PORTA=0,
  PORTB=1,
  PORTC=2,
  PORTD=3,
} EPortType;

/* Types used for the table below */
typedef struct _PinDescription
{
  EPortType       ulPort;
  uint32_t        ulPin;
} PinDescription;

/* Pins table to be instantiated */
extern const PinDescription sdhc0_pin_description [];
extern const PinDescription sdhc1_pin_description [];
extern PinDescription *g_APinDescription;

void pin_mode ( uint32_t ulPin, uint32_t ulMode );
void digital_write ( uint32_t ulPin, uint32_t ulVal );
int digital_read ( uint32_t ulPin );
void set_pin_to_pio_sdhc (int ulPin);

unsigned long vulintus_micros(void);
void vulintus_delay(unsigned long ms);

#endif /* __VULINTUS_PIN_FUNCTIONS */