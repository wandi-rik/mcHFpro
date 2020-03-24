/************************************************************************************
**                                                                                 **
**                             mcHF Pro QRP Transceiver                            **
**                         Krassi Atanassov - M0NKA 2012-2019                      **
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
#ifndef __UI_MENU_H
#define __UI_MENU_H

#define FOOTER_EDIT_X			20
#define FOOTER_EDIT_Y			443
#define FOOTER_EDIT_X_SIZE		260
#define FOOTER_EDIT_Y_SIZE		30

void    ui_init_menu(void);
void 	ui_destroy_menu(void);
void 	ui_periodic_processes(void);
void    ui_set_gui_profile(void);

#endif
