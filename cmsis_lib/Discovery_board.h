#ifndef DISCOVERY_BOARD_H_
#define DISCOVERY_BOARD_H_

#include "stm32f4xx.h"
#include "stm32f4xx_i2c_extension.h"

//Periph clocks of GPIO port and of Peripheral
#define I2C_APB1Periph     RCC_APB1Periph_I2C2
#define I2C_AHB1Periph     RCC_AHB1Periph_GPIOB
//Alternate function source (NOT USED IN EVERY STM32Fxxx)
#define I2C_GPIO_AF        GPIO_AF_I2C2
#define I2C_PINS_SCL       GPIO_PinSource10
#define I2C_PINS_SDA       GPIO_PinSource11
//Chosen I2C Peripheral
#define I2C_USED           I2C2
#define I2C_PORT           GPIOB
#define	I2C_SCL_PIN        GPIO_Pin_10
#define I2C_SDA_PIN        GPIO_Pin_11


#endif
