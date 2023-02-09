#ifndef PERIPHERAL_CLK_CONFIG_H
#define PERIPHERAL_CLK_CONFIG_H

#include <stdint.h>

extern uint8_t CONF_GCLK_SDHC_SRC;

extern uint32_t CONF_SDHC_FREQUENCY;

extern uint8_t CONF_GCLK_SDHC_SRC_SLOW;

extern uint32_t CONF_SDHC_FREQUENCY_SLOW;

//Clock Generator Select
//This defines the clock generator mode in the SDCLK Frequency Select field
//0= Divided Clock mode
//1= Programmable Clock mode
extern int32_t CONF_SDHC_CLK_GEN_SEL;

#endif // PERIPHERAL_CLK_CONFIG_H
