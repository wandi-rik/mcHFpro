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
#ifndef UI_CONTROLS_FILTER_H
#define UI_CONTROLS_FILTER_H

#define	FIL_BKG_COLOR			GUI_LIGHTBLUE

#define FIL_BTN_X				45
#define FIL_BTN_SHFT			66

// Exports
void ui_controls_filter_init(void);
void ui_controls_filter_quit(void);

void ui_controls_filter_touch(void);
void ui_controls_filter_refresh(void);

#endif
