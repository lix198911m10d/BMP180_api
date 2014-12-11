#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "Discovery_board.h"
#include "stm32f4xx_i2c_extension.h"
#include "bmp180_api.h"

int main(void){
	GPIO_InitTypeDef gpioinit;
	I2C_InitTypeDef i2cinit;
	unsigned char BMP_address= 0xEE;
	unsigned char ID_Reg= 0xAA;
	unsigned char answer[6];
	unsigned short i, k;
	BMP180_CAL_TypeDef calregs;
	BMP180_CTRL_TypeDef ctrlregs;
	short temperature;
	long ut, up, pressure;
	long pr_avg=0;
	unsigned char oss=BMP180_OSS_ULTRAHIGHRES;


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_HSEConfig(RCC_HSE_ON);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
	i=RCC_WaitForHSEStartUp();

	gpioinit.GPIO_Mode=GPIO_Mode_AF;
	gpioinit.GPIO_OType=GPIO_OType_OD;
	gpioinit.GPIO_PuPd=GPIO_PuPd_UP;
	gpioinit.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &gpioinit);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);

	I2C_StructInit(&i2cinit);
	i2cinit.I2C_ClockSpeed=50000;
	I2C_Init(I2C2, &i2cinit);
	I2C_Cmd(I2C2, ENABLE);

	i=BMP180_READ_CALDATA(&calregs);

	while(1){
		for(k=0;k<8;k++){
			i=BMP180_READ_UT(&ctrlregs);
			ut=(ctrlregs.MSB<<8) | ctrlregs.LSB;
			temperature=BMP180_CALC_TEMP(ut, &calregs);

			//ctrlregs.CTRL=BMP180_COM_CTRL_MEAS_PRESS|(BMP180_OSS_ULTRAHIGHRES<<BMP180_BIT_CTRL_MEAS_OSS_SHIFT);
			i=BMP180_READ_UP(&ctrlregs, oss);
			up=(ctrlregs.MSB<<16 | ctrlregs.LSB<<8 |ctrlregs.XLSB )>>(8-oss);
			pressure=BMP180_CALC_PRESS(up, &calregs, oss);
			pr_avg+=pressure;
		}
		pr_avg/=8;
		pr_avg=0;
	}

	while(1){

	}
}
