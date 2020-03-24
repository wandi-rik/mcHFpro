/************************************************************************************
**                                                                                 **
**                             mcHF Pro QRP Transceiver                            **
**                         Krassi Atanassov - M0NKA 2012-2020                      **
**                            mail: djchrismarc@gmail.com                          **
**                                 twitter: @bph_co                                **
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
#ifndef __ICC_PROC_H
#define __ICC_PROC_H

// -----------------------------------------------------------------------
// List of ICC commands
//
#define ICC_BROADCAST					0
#define ICC_START_I2S_PROC				1
#define ICC_TOGGLE_LED					2

// -----------------------------------------------------------------------

void icc_proc_hw_init(void);
void icc_proc_task(void const *arg);

#endif
