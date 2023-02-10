#include <stdint.h>
#include "sdhc_driver_init.h"
#include "sd_mmc/sd_mmc.h"
#include "config/peripheral_clock_config.h"
#include "config/conf_sd_mmc.h"
#include "vulintus_pin_functions.h"

struct mci_sync_desc IO_BUS;
Sdhc *sdhc_instance;
uint8_t sdhc_gclk_id;
uint8_t sdhc_gclk_id_slow;
uint32_t sdhc_gclk_source;
uint32_t sdhc_gclk_source_slow;

/* Card Detect (CD) pin settings */
static sd_mmc_detect_t SDMMC_cd[CONF_SD_MMC_MEM_CNT] = {

    {-1, CONF_SD_MMC_0_CD_DETECT_VALUE},
};

/* Write Protect (WP) pin settings */
static sd_mmc_detect_t SDMMC_wp[CONF_SD_MMC_MEM_CNT] = {

    {-1, CONF_SD_MMC_0_WP_DETECT_VALUE},
};

void IO_BUS_PORT_init()
{
    for (int i = 0; i < 6; i++)
    {
        pin_mode(i, 1);
        digital_write(i, 0);
        set_pin_to_pio_sdhc(i);
    }
}

void IO_BUS_CLOCK_init(void)
{
	hri_mclk_set_AHBMASK_SDHC0_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, sdhc_gclk_id, sdhc_gclk_source);
	hri_gclk_write_PCHCTRL_reg(GCLK, sdhc_gclk_id_slow, sdhc_gclk_source_slow);
}

int IO_BUS_init(int sdhc_index)
{
    int result = 0;

    if (sdhc_index == 1)
    {
        //The user has specified SDHC1
        sdhc_instance = SDHC1;
        sdhc_gclk_id = SDHC1_GCLK_ID;
        sdhc_gclk_id_slow = SDHC1_GCLK_ID_SLOW;
        g_APinDescription = sdhc1_pin_description;
    }
    else
    {
        //The user has specified SDHC0
        sdhc_instance = SDHC0;
        sdhc_gclk_id = SDHC0_GCLK_ID;
        sdhc_gclk_id_slow = SDHC0_GCLK_ID_SLOW;
        g_APinDescription = sdhc0_pin_description;
    }
    
    sdhc_gclk_source = CONF_GCLK_SDHC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    sdhc_gclk_source_slow = CONF_GCLK_SDHC_SRC_SLOW | (1 << GCLK_PCHCTRL_CHEN_Pos);        

    IO_BUS_CLOCK_init();
    mci_sync_init(&IO_BUS, sdhc_instance);
    IO_BUS_PORT_init();

    return result;
}

int sdhc_driver_init(int gclk_source, uint32_t gclk_frequency, int gclk_source_slow, uint32_t gclk_frequency_slow, int sdhc_index)
{
    //Get the clock source based on the user's input
    CONF_GCLK_SDHC_SRC = gclk_source;
    CONF_GCLK_SDHC_SRC_SLOW = gclk_source_slow;
    
    //Get the clock frequency based on the user's input
    CONF_SDHC_FREQUENCY = gclk_frequency;
    CONF_SDHC_FREQUENCY_SLOW = gclk_frequency_slow;

    //Initialize the IO bus
    int result = IO_BUS_init(sdhc_index);

    //Initialize the SD/MMC stack with the card-detect and write-detect pins
    if (result == 0)
    {
        sd_mmc_init(&IO_BUS, SDMMC_cd, SDMMC_wp);
    }
	
    //Return a result to the caller
    return result;
}

