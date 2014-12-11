/*#include "Discovery_board.h"

unsigned char BMP_READ(unsigned char device_addr, unsigned char reg_addr, unsigned char *reg_data, unsigned char cnt){
	I2C_Read(I2C_USEDPORT, device_addr, reg_addr, *reg_data, cnt);
}
unsigned char BMP_WRITE(unsigned char device_addr, unsigned char reg_addr, unsigned char reg_data[], unsigned char cnt){
	I2C_Write(I2C_USEDPORT, device_addr, reg_addr, reg_data, cnt);
}
void Delay(unsigned int msec){
	Delay_ApproxMilSec(msec);
}
*/
