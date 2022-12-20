#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define VENDOR_NAME "Vulintus"
#define PRODUCT_NAME "OmniTrak"
#define VOLUME_LABEL "GCM4BOOT"
#define INDEX_URL "http://adafru.it/4064"
#define BOARD_ID "SAMD51P20A-OmniTrak-v2"

#define USB_VID 0x239A
#define USB_PID 0x0031

//#define LED_PIN PIN_PB01
//#define LED_TX_PIN PIN_PC30
//#define LED_RX_PIN PIN_PC31

//#define BOARD_NEOPIXEL_PIN PIN_PC24
#define BOARD_NEOPIXEL_COUNT 0

#define BOOT_USART_MODULE                 SERCOM0
#define BOOT_USART_MASK                   APBAMASK
#define BOOT_USART_BUS_CLOCK_INDEX        MCLK_APBAMASK_SERCOM0
#define BOOT_USART_PAD_SETTINGS           UART_RX_PAD1_TX_PAD0
#define BOOT_USART_PAD3                   PINMUX_UNUSED
#define BOOT_USART_PAD2                   PINMUX_UNUSED
#define BOOT_USART_PAD1                   PINMUX_PB25C_SERCOM0_PAD1
#define BOOT_USART_PAD0                   PINMUX_PB24C_SERCOM0_PAD0
#define BOOT_GCLK_ID_CORE                 SERCOM0_GCLK_ID_CORE
#define BOOT_GCLK_ID_SLOW                 SERCOM0_GCLK_ID_SLOW

#endif
