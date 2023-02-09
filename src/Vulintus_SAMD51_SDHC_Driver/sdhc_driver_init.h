#ifndef __VULINTUS_SDHC_DRIVER_INIT_H
#define __VULINTUS_SDHC_DRIVER_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sam.h"
#include <stdint.h>
#include "asf4/hal/hal_atomic.h"
#include "asf4/hal/hal_mci_sync.h"

extern struct mci_sync_desc IO_BUS;
extern Sdhc *sdhc_instance;
extern uint8_t sdhc_gclk_id;
extern uint8_t sdhc_gclk_id_slow;
extern uint32_t sdhc_gclk_source;
extern uint32_t sdhc_gclk_source_slow;

/**
 * \brief Initialize SD MMC Stack
 */
int sdhc_driver_init(int gclk_source, uint32_t gclk_frequency, 
                     int gclk_source_slow, uint32_t gclk_frequency_slow, 
                     int clk_pin, int cmd_pin, int dat0_pin, int dat1_pin, int dat2_pin, int dat3_pin);

#ifdef __cplusplus
}
#endif

#endif // __VULINTUS_SDHC_DRIVER_INIT_H
