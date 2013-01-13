
#ifndef GPIO_UTIL_H
#define GPIO_UTIL_H

#include <stdbool.h>
#include <stdint.h>

void gpio_select_function(uint8_t gpio, uint8_t func);
void gpio_reg_set(volatile uint32_t* reg, uint8_t bit);
void gpio_reg_clear(volatile uint32_t* reg, uint8_t bit);

#define PAD_DIR_INPUT 0
#define PAD_DIR_OUTPUT 1
void gpio_set_pad_dir(uint8_t gpio, uint8_t dir);

#undef gpio_set
#undef gpio_reset
#undef gpio_read

//#define gpio_set gpio_set_ian
//#define gpio_reset gpio_reset_ian
//#define gpio_read gpio_read_ian

void gpio_set(uint8_t gpio);
void gpio_reset(uint8_t gpio);
bool gpio_read(uint8_t gpio);

/*
//trm added
#define SSI_TX 		GPIO_00
#define SSI_RX 		GPIO_01
#define SSI_FSYN 	GPIO_02
#define SSI_BITCK 	GPIO_03
#define SPI_SS		GPIO_04
#define SPI_MISO	GPIO_05
#define SPI_MOSI	GPIO_06
#define SPI_SCK		GPIO_07
#define TMR_0		GPIO_08
#define TMR_1		GPIO_09
#define TMR_2		GPIO_10
#define TMR_3		GPIO_11
#define I2CSCL		GPIO_12
#define I2CSDA		GPIO_13
#define UART1_TX	GPIO_14
#define	UART1_RX	GPIO_15
#define UART1_CTS	GPIO_16
#define UART1_RTS	GPIO_17
#define UART2_TX	GPIO_18
#define UART2_RX	GPIO_19
#define UART2_CTS	GPIO_20
#define UART2_RTS	GPIO_21
#define KBI_0 		GPIO_22
#define KBI_1 		GPIO_23
#define KBI_2 		GPIO_24
#define KBI_3 		GPIO_25
#define KBI_4 		GPIO_26
#define KBI_5 		GPIO_27
#define KBI_6 		GPIO_28
#define KBI_7 		GPIO_29
#define ADC_0		GPIO_30
#define ADC_1		GPIO_31
#define ADC_2		GPIO_32
#define ADC_3		GPIO_33
#define ADC_4		GPIO_34
#define ADC_5		GPIO_35
#define ADC_6		GPIO_36
#define ADC_7		GPIO_37
#define ADC2_VREFH	GPIO_38
#define ADC_2_VREFH	GPIO_38
#define ADC_2VREFH	GPIO_38
#define ADC2_VREFL	GPIO_39
#define ADC_2_VREFL	GPIO_39
#define ADC_2VREFL	GPIO_39
#define ADC1_VREFH	GPIO_40
#define ADC_1_VREFH	GPIO_40
#define ADC_1VREFH	GPIO_40
#define ADC1_VREFL	GPIO_41
#define ADC_1_VREFL	GPIO_41
#define ADC_1VREFL	GPIO_41
#define ANT_1		GPIO_42
#define ANT_2		GPIO_43
#define TX_ON		GPIO_44
#define	RX_ON		GPIO_45
*/

#define GPIO_SETUP_OUTPUT(x) GPIO->PAD_DIR.x = PAD_DIR_OUTPUT;
#define GPIO_SETUP_INPUT(x)  GPIO->PAD_DIR.x = PAD_DIR_INPUT;
#define GPIO_SET(x) GPIO->DATA_SET.x = 1;
#define GPIO_RESET(x) GPIO->DATA_RESET.x = 1;

#define GPIO_OFF(x) GPIO_RESET(x)
#define GPIO_ON(x)  GPIO_SET(x)

#define GPIO_READ(x) GPIO->DATA.x
#endif
