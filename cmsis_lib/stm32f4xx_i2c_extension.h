#ifndef STM32F4XX_I2C_EXTENSION_H
#define STM32F4XX_I2C_EXTENSION_H

#include "Discovery_board.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_rcc.h"

#define I2C_TIMEOUT_MAX 500
#define SUCCESSFUL      0
#define FAILED          1

unsigned char I2C_Read(I2C_TypeDef* I2Cx, unsigned char device_addr, unsigned char reg_addr, unsigned char *reg_data, unsigned char cnt);
unsigned char I2C_Write(I2C_TypeDef* I2Cx, unsigned char device_addr, unsigned char reg_addr, unsigned char reg_data[]);
void Delay_Approx100uSec(unsigned int msec);

#endif
