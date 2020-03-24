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
#ifndef UI_CONTROLS_CPU_STAT_H
#define UI_CONTROLS_CPU_STAT_H

// ----------------------------------------------------------------------------
// Still alive blinker
//
#define BLINKER_X					(CPU_L_X + 5)
#define BLINKER_Y					(CPU_L_Y + 4)

// Exports
void ui_controls_cpu_stat_init(void);
void ui_controls_cpu_stat_quit(void);

void ui_controls_cpu_stat_touch(void);
void ui_controls_cpu_stat_refresh(void);

#endif
