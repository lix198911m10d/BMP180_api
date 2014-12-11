#include "bmp180_api.h"
#include "Discovery_board.h"

//reads and stores the calibration register values
//the function stores the calibration data into "reg_data" (given as passed parameter)
//input: BMP180_CAL_TypeDef* reg_data
unsigned char BMP180_READ_CALDATA(BMP180_CAL_TypeDef* reg_data){
	unsigned char output;
	unsigned char data[22];

	//reading out the calibration registers
	output = BMP_READ(I2C_USED, BMP180_DEV_ADDR, BMP180_CALREGS_BASE_ADDRESS, data, 22);

	/*parameters AC1-AC6*/
	   reg_data->AC1 =  (data[0] << 8) | data[1];
	   reg_data->AC2 =  (data[2] << 8) | data[3];
	   reg_data->AC3 =  (data[4] << 8) | data[5];
	   reg_data->AC4 =  (data[6] << 8) | data[7];
	   reg_data->AC5 =  (data[8] << 8) | data[9];
	   reg_data->AC6 = (data[10] << 8) | data[11];

	   /*parameters B1,B2*/
	   reg_data->B1 =  (data[12] << 8) | data[13];
	   reg_data->B2 =  (data[14] << 8) | data[15];

	   /*parameters MB,MC,MD*/
	   reg_data->MB =  (data[16] << 8) | data[17];
	   reg_data->MC =  (data[18] << 8) | data[19];
	   reg_data->MD =  (data[20] << 8) | data[21];
	return output;
}

//Reads out the uncompensated temperature value
//input: BMP180_CTRL_TypeDef* result
//     the uncompensated temperature value will be stored in result->MSB and result->LSB
//output: unsigned char output
//     output == SUCCESSFUL -> the conversion was successful
//     output == FAILED -> the converion faild
//     the values of SUCCESSFUL and FAILD are defined in "stm32fxx_i2c_extension.h"
unsigned char BMP180_READ_UT(BMP180_CTRL_TypeDef* result){
	unsigned char output;
	unsigned char data[2];

	data[0]=BMP180_COM_CTRL_MEAS_TEMP;
	output = BMP_WRITE(I2C_USED, BMP180_DEV_ADDR, BMP180_ADDR_CTRL_MEAS, data);
	if(output==FAILED)
		return output;

	Delay(BMP180_WAIT_UT);

	output = BMP_READ(I2C_USED, BMP180_DEV_ADDR, BMP180_ADDR_OUT_MSB, data, 2);
	result->MSB=data[0];
	result->LSB=data[1];

	return output;
}

//calculates valid temperature from uncompnsated temperature
//input: - unsigned long up: uncompensated temperature
//       - BMP180_CAL_TypeDef *reg_data: calibration register values
//output: short, the calculated temperature in 0.1C°
short BMP180_CALC_TEMP(unsigned long ut,BMP180_CAL_TypeDef *reg_data){
	short temperature;
	long x1, x2;
	x1=(((long)ut - (long)reg_data->AC6) * (long)reg_data->AC5) >> 15;
	x2=((long)reg_data->MC << 11) / (x1 + reg_data->MD);
	reg_data->B5 = x1+x2;
	temperature = (reg_data->B5+8) >> 4;
	return temperature;
}

//Reads out the uncompensated pressure value
//input: - BMP180_CTRL_TypeDef* result
//       - unsigned char oss: oversapmling setting
//             typical values of oss:     BMP180_OSS_LOWPOWER
//                                        BMP180_OSS_STANDARD
//                                        BMP180_OSS_HIGHRES
//                                        BMP180_OSS_ULTRAHIGHRES
//     the uncompensated pressure value will be stored in result->MSB, result->LSB and result->XLSB
//output: unsigned char output
//     output == SUCCESSFUL -> the conversion was successful
//     output == FAILED -> the converion faild
//     the values of SUCCESSFUL and FAILD are defined in "stm32fxx_i2c_extension.h"
unsigned char BMP180_READ_UP(BMP180_CTRL_TypeDef* result, unsigned char oss){
	unsigned char output;
	unsigned char data[3];

	data[0]=BMP180_COM_CTRL_MEAS_PRESS|(oss<<BMP180_BIT_CTRL_MEAS_OSS_SHIFT);
	output = BMP_WRITE(I2C_USED, BMP180_DEV_ADDR, BMP180_ADDR_CTRL_MEAS, data);
	if(output==FAILED)
		return output;

	Delay((BMP180_WAIT_UP_LOWPOWER-30)+(30<<oss));

	output = BMP_READ(I2C_USED, BMP180_DEV_ADDR, BMP180_ADDR_OUT_MSB, data, 3);
	result->MSB=data[0];
	result->LSB=data[1];
	result->XLSB=data[2];

	return output;
}

//calculates valid pressure from uncompnsated pressure
//input: - unsigned long up: uncompensated pressure
//       - BMP180_CAL_TypeDef *reg_data: calibration register values
//       - unsigned char oss
//             typical values of oss:     BMP180_OSS_LOWPOWER
//                                        BMP180_OSS_STANDARD
//                                        BMP180_OSS_HIGHRES
//                                        BMP180_OSS_ULTRAHIGHRES
//output: long, the calculated pressure in Pa
long BMP180_CALC_PRESS(unsigned long up,BMP180_CAL_TypeDef *reg_data, unsigned char oss){
	long pressure, x1, x2, x3, b3, b6;
	unsigned long b4, b7;

	b6=reg_data->B5 - 4000;

	/*****calculate B3************/
	x1 = (b6*b6) >> 12;
	x1 *= reg_data->B2;
	x1 >>= 11;
	x2 = (reg_data->AC2*b6);
	x2 >>= 11;
	x3 = x1 + x2;
	b3 = ((((((long)reg_data->AC1)<<2) + x3) << oss)+2) >> 2;

	/*****calculate B4************/
	x1 = (reg_data->AC3 * b6) >> 13;
	x2 = (reg_data->B1 * ((b6*b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (reg_data->AC4 * (unsigned long) (x3 + 32768)) >> 15;
	b7 = ((unsigned long)(up - b3) * (50000>>oss));
	if (b7 < 0x80000000)
		pressure = (b7 << 1) / b4;
	else
		pressure = (b7 / b4) << 1;
	x1 = pressure >> 8;
	x1 *= x1;
	x1 = (x1 * 3038) >> 16;
	x2 = (pressure * -7357) >> 16;
	pressure += (x1 + x2 + 3791) >> 4;/* pressure in Pa*/
	return pressure;
}
