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
#ifndef UI_CONTROLS_SPECTRUM_H
#define UI_CONTROLS_SPECTRUM_H

#include "GUI.h"
//#include "arm_math.h"

// Definitions in pixels,not Hz!
#define SPECTRUM_MID_POINT			400
#define SPECTRUM_DEF_HALF_BW		20

void ui_controls_spectrum_init(void);
void ui_controls_spectrum_quit(void);

void ui_controls_spectrum_touch(void);
void ui_controls_spectrum_refresh(FAST_REFRESH *cb);

#endif
