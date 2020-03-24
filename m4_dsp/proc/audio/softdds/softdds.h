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

#ifndef __SOFTDDS_H
#define __SOFTDDS_H

#include "arm_math.h"

// 256 points
//#define DDS_ACC_SHIFT		8

// 512 points
//#define DDS_ACC_SHIFT		7

// 1024 points
#define DDS_ACC_SHIFT		6

// Soft DDS public structure
typedef struct SoftDds
{
	// DDS accumulator
	ulong 	acc;

	// DDS step - not working if part of the structure
	ulong 	step;

} SoftDds;

void 	softdds_setfreq(float freq, ulong samp_rate, uchar smooth);
void 	softdds_runf(float32_t *i_buff, float32_t *q_buff, ushort size);

#endif

