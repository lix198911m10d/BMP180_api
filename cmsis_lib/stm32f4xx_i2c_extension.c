#include "stm32f4xx_i2c_extension.h"

//The function performs a whole reading operation through I2C
//input:
//	I2Cx: chosen I2C periphery: I2C1/2/3
//	device_addr: ID of I2C slave unit
//	reg_addr: Register address to read
//	reg_data[]: destination register, this one will contain the read content
//	cnt: number of registers to read
//output:
//	0 = Reading was successful
//	1 = Reading was not successful
unsigned char I2C_Read(I2C_TypeDef* I2Cx, unsigned char device_addr, unsigned char reg_addr, unsigned char reg_data[], unsigned char cnt){
	unsigned i=0;
	unsigned char output=SUCCESSFUL;
	unsigned short timeout = I2C_TIMEOUT_MAX;
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)){
		if((timeout--) == 0){
			output=FAILED;
			break;
		}
	}
	I2C_Send7bitAddress(I2Cx, device_addr, I2C_Direction_Transmitter);
	timeout = I2C_TIMEOUT_MAX;
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
		if((timeout--) == 0){
			output=FAILED;
			break;
		}
	}
	I2C_SendData(I2Cx, reg_addr);
	timeout = I2C_TIMEOUT_MAX;
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
		if((timeout--) == 0){
			output=FAILED;
			break;
		}
	}
	I2C_GenerateSTART(I2Cx, ENABLE);
	timeout = I2C_TIMEOUT_MAX;
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)){
		if((timeout--) == 0){
			output=FAILED;
			break;
		}
	}
	I2C_Send7bitAddress(I2Cx, device_addr, I2C_Direction_Receiver);
	timeout = I2C_TIMEOUT_MAX;
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){
		if((timeout--) == 0){
			output=FAILED;
			break;
		}
	}
	for(I2C_AcknowledgeConfig(I2Cx, ENABLE) ; i<cnt ; i++){
		if(cnt-i==1)
			I2C_AcknowledgeConfig(I2Cx, DISABLE);
		timeout = I2C_TIMEOUT_MAX;
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)){
			if((timeout--) == 0){
				output=FAILED;
				break;
			}
		}
		reg_data[i]=I2C_ReceiveData(I2Cx);
	}
	I2C_GenerateSTOP(I2Cx, ENABLE);
	return output;
}

//The function performs a whole writing operation through I2C
//input:
//	I2Cx: chosen I2C periphery: I2C1/2/3
//	device_addr: ID of I2C slave unit
//	reg_addr: Register address to write
//	reg_data[]: source register, this one contains the data to write
//output:
//	0 = Reading was successful
//	1 = Reading was not successful
unsigned char I2C_Write(I2C_TypeDef* I2Cx, unsigned char device_addr, unsigned char reg_addr, unsigned char reg_data[]){
	unsigned i=0;
	unsigned char output=SUCCESSFUL;
	unsigned short timeout = I2C_TIMEOUT_MAX;
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)){
		if((timeout--) == 0){
			output=FAILED;
			break;
		}
	}
	I2C_Send7bitAddress(I2Cx, device_addr, I2C_Direction_Transmitter);
	timeout = I2C_TIMEOUT_MAX;
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
		if((timeout--) == 0){
			output=FAILED;
			break;
		}
	}
	I2C_SendData(I2Cx, reg_addr);
	timeout = I2C_TIMEOUT_MAX;
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
		if((timeout--) == 0){
			output=FAILED;
			break;
		}
	}
	I2C_SendData(I2Cx, reg_data[i]);
	timeout = I2C_TIMEOUT_MAX;
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
		if((timeout--) == 0){
			output=FAILED;
			break;
		}
	}
	I2C_GenerateSTOP(I2Cx, ENABLE);
	return output;
}

//Delays approximately msec*ms time
//The function DOES NOT use timer so it is kinda shitty function
void Delay_Approx100uSec(unsigned int msec){
	unsigned int counttil, i=0;
	RCC_ClocksTypeDef RCC_Clocks;

	i=RCC_GetSYSCLKSource();
	RCC_GetClocksFreq(&RCC_Clocks);
	counttil=(RCC_Clocks.HCLK_Frequency/10000)*msec/12;
	for(i=0; i<counttil; i++)
		asm("nop");
}

