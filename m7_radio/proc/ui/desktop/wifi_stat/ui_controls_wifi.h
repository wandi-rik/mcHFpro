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
#ifndef __UI_CONTROLS_WIFI_H
#define __UI_CONTROLS_WIFI_H

// Exports
void ui_controls_wifi_init(WM_HWIN hParent);
void ui_controls_wifi_quit(void);

void ui_controls_wifi_touch(void);
void ui_controls_wifi_refresh(void);

#endif
