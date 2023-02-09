#include <stdint.h>
#include "sdhc_driver_init.h"
#include "sd_mmc/sd_mmc.h"
#include "config/peripheral_clock_config.h"
#include "config/conf_sd_mmc.h"

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

void set_pin_to_pio_sdhc (int ulPin)
{
    if ( g_APinDescription[ulPin].ulPin & 1 ) // is pin odd?
    {
        uint32_t temp;

        // Get whole current setup for both odd and even pins and remove odd one
        temp = (PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg) & PORT_PMUX_PMUXE( 0xF ) ;
        // Set new muxing
        PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg = temp|PORT_PMUX_PMUXO( PIO_SDHC ) ;
        // Enable port mux
        PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg |= PORT_PINCFG_PMUXEN | PORT_PINCFG_DRVSTR;
    }
    else // even pin
    {
        uint32_t temp;

        temp = (PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg) & PORT_PMUX_PMUXO( 0xF ) ;
        PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg = temp|PORT_PMUX_PMUXE( PIO_SDHC ) ;
        PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg |= PORT_PINCFG_PMUXEN | PORT_PINCFG_DRVSTR ; // Enable port mux
    }    
}

void IO_BUS_PORT_init(int clk_pin, int cmd_pin, int dat0_pin, int dat1_pin, int dat2_pin, int dat3_pin)
{
    pinMode(clk_pin, OUTPUT);
	digitalWrite(clk_pin, LOW);
    set_pin_to_pio_sdhc(clk_pin);

    pinMode(cmd_pin, OUTPUT);
    digitalWrite(cmd_pin, LOW);
    set_pin_to_pio_sdhc(cmd_pin);

	pinMode(dat0_pin, OUTPUT);
    digitalWrite(dat0_pin, LOW);
    set_pin_to_pio_sdhc(dat0_pin);

    pinMode(dat1_pin, OUTPUT);
    digitalWrite(dat1_pin, LOW);
    set_pin_to_pio_sdhc(dat1_pin);

    pinMode(dat2_pin, OUTPUT);
    digitalWrite(dat2_pin, LOW);
    set_pin_to_pio_sdhc(dat2_pin);

    pinMode(dat3_pin, OUTPUT);
    digitalWrite(dat3_pin, LOW);
    set_pin_to_pio_sdhc(dat3_pin);
}

void IO_BUS_CLOCK_init(void)
{
	hri_mclk_set_AHBMASK_SDHC0_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, sdhc_gclk_id, sdhc_gclk_source);
	hri_gclk_write_PCHCTRL_reg(GCLK, sdhc_gclk_id_slow, sdhc_gclk_source_slow);
}

int IO_BUS_init(int clk_pin, int cmd_pin, int dat0_pin, int dat1_pin, int dat2_pin, int dat3_pin)
{
    int result = 0;

    //We must make sure the pins being used are valid pins for SDHC0 or SDHC1 on the SAMD51
    EPortType sdhc_clk_port = g_APinDescription[clk_pin].ulPort;
    uint32_t sdhc_clk_pin = g_APinDescription[clk_pin].ulPin;

    EPortType sdhc_cmd_port = g_APinDescription[cmd_pin].ulPort;
    uint32_t sdhc_cmd_pin = g_APinDescription[cmd_pin].ulPin;

    EPortType sdhc_dat0_port = g_APinDescription[dat0_pin].ulPort;
    uint32_t sdhc_dat0_pin = g_APinDescription[dat0_pin].ulPin;

    EPortType sdhc_dat1_port = g_APinDescription[dat1_pin].ulPort;
    uint32_t sdhc_dat1_pin = g_APinDescription[dat1_pin].ulPin;

    EPortType sdhc_dat2_port = g_APinDescription[dat2_pin].ulPort;
    uint32_t sdhc_dat2_pin = g_APinDescription[dat2_pin].ulPin;

    EPortType sdhc_dat3_port = g_APinDescription[dat3_pin].ulPort;
    uint32_t sdhc_dat3_pin = g_APinDescription[dat3_pin].ulPin;

    bool is_sdhc0 = (sdhc_clk_port == PORTB) &&
                    (sdhc_clk_pin == 11) &&

                    (sdhc_cmd_port == PORTA) &&
                    (sdhc_cmd_pin == 8) &&

                    (sdhc_dat0_port == PORTA) &&
                    (sdhc_dat0_pin == 9) &&

                    (sdhc_dat1_port == PORTA) &&
                    (sdhc_dat1_pin == 10) &&

                    (sdhc_dat2_port == PORTA) &&
                    (sdhc_dat2_pin == 11) &&

                    (sdhc_dat3_port == PORTB) &&
                    (sdhc_dat3_pin == 10);

    bool is_sdhc1 = (sdhc_clk_port == PORTA) &&
                    (sdhc_clk_pin == 21) &&

                    (sdhc_cmd_port == PORTA) &&
                    (sdhc_cmd_pin == 20) &&

                    (sdhc_dat0_port == PORTB) &&
                    (sdhc_dat0_pin == 18) &&

                    (sdhc_dat1_port == PORTB) &&
                    (sdhc_dat1_pin == 19) &&

                    (sdhc_dat2_port == PORTB) &&
                    (sdhc_dat2_pin == 20) &&

                    (sdhc_dat3_port == PORTB) &&
                    (sdhc_dat3_pin == 21);

    if (is_sdhc0 || is_sdhc1)
    {
        if (is_sdhc0)
        {
            //The user has specified SDHC0
            sdhc_instance = SDHC0;
            sdhc_gclk_id = SDHC0_GCLK_ID;
            sdhc_gclk_id_slow = SDHC0_GCLK_ID_SLOW;
        }
        else if (is_sdhc1)
        {
            //The user has specified SDHC1
            sdhc_instance = SDHC1;
            sdhc_gclk_id = SDHC1_GCLK_ID;
            sdhc_gclk_id_slow = SDHC1_GCLK_ID_SLOW;                        
        }

        sdhc_gclk_source = CONF_GCLK_SDHC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
        sdhc_gclk_source_slow = CONF_GCLK_SDHC_SRC_SLOW | (1 << GCLK_PCHCTRL_CHEN_Pos);        

        IO_BUS_CLOCK_init();
        mci_sync_init(&IO_BUS, sdhc_instance);
        IO_BUS_PORT_init(clk_pin, cmd_pin, dat0_pin, dat1_pin, dat2_pin, dat3_pin);     
    }
    else
    {
        //error state: incorrect pins
        sdhc_instance = NULL;

        //Set the result to -1 to indicate an error
        result = -1;
    }

    return result;
}

int sdhc_driver_init(int gclk_source, uint32_t gclk_frequency, int gclk_source_slow, uint32_t gclk_frequency_slow, int clk_pin, int cmd_pin, int dat0_pin, int dat1_pin, int dat2_pin, int dat3_pin)
{
    //Get the clock source based on the user's input
    CONF_GCLK_SDHC_SRC = gclk_source;
    CONF_GCLK_SDHC_SRC_SLOW = gclk_source_slow;
    
    //Get the clock frequency based on the user's input
    CONF_SDHC_FREQUENCY = gclk_frequency;
    CONF_SDHC_FREQUENCY_SLOW = gclk_frequency_slow;

    //Initialize the IO bus
    int result = IO_BUS_init(clk_pin, cmd_pin, dat0_pin, dat1_pin, dat2_pin, dat3_pin);

    //Initialize the SD/MMC stack with the card-detect and write-detect pins
    if (result == 0)
    {
        sd_mmc_init(&IO_BUS, SDMMC_cd, SDMMC_wp);
    }
	
    //Return a result to the caller
    return result;
}

