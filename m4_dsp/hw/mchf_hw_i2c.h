/************************************************************************************
**                                                                                 **
**                             mcHF Pro QRP Transceiver                            **
**                         Krassi Atanassov - M0NKA 2012-2019                      **
**                            mail: djchrismarc@gmail.com                          **
**                                 twitter: @M0NKA_                                **
**---------------------------------------------------------------------------------**
**                                                                                 **
**  File name:                                                                     **
**  Description:                                                                   **
**  Last Modified:                                                                 **
**  Licence:                                                                       **
**          The mcHF project is released for radio amateurs experimentation,       **
**          non-commercial use only. All source files under GPL-3.0, unless        **
**          third party drivers specifies otherwise. Thank you!                    **
************************************************************************************/

#ifndef __MCHF_HW_I2C_H
#define __MCHF_HW_I2C_H

void 	mchf_hw_i2c_init(void);
void 	mchf_hw_i2c_reset(void);

uchar 	mchf_hw_i2c_WriteRegister(uchar I2CAddr,uchar RegisterAddr, uchar RegisterValue);
uchar 	mchf_hw_i2c_WriteBlock(uchar I2CAddr,uchar RegisterAddr, uchar *data,ulong size);
uchar 	mchf_hw_i2c_ReadRegister (uchar I2CAddr,uchar RegisterAddr, uchar *RegisterValue);
uchar 	mchf_hw_i2c_ReadData(uchar I2CAddr,uchar RegisterAddr, uchar *data, ulong size);

#endif
