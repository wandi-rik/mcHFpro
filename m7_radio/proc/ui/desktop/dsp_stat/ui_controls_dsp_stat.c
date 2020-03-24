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

#include "mchf_pro_board.h"
#include "main.h"

#ifdef CONTEXT_DRIVER_UI

#include "gui.h"
#include "dialog.h"
//#include "ST_GUI_Addons.h"

#include "ui_controls_dsp_stat.h"
#include "desktop\ui_controls_layout.h"

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

uchar dsp_control_init_done = 0;
uchar skip_dsp_check = 0;

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_dsp_stat_print
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void ui_controls_dsp_stat_print(void)
{
	//char   	buff[100];

	skip_dsp_check++;
	if(skip_dsp_check < 4)
		return;

	skip_dsp_check = 0;

	if(!dsp_control_init_done)
	{
		// Destroy control
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect(			DSP_POS_X,		DSP_POS_Y,	DSP_POS_X + 80,	DSP_POS_Y + 14		 );

		// Create frame
		GUI_SetColor(GUI_BLUE);
		GUI_DrawRoundedRect(	DSP_POS_X,		DSP_POS_Y,	DSP_POS_X + 80,	DSP_POS_Y + 14,		2);
		GUI_FillRect(			DSP_POS_X + 35,	DSP_POS_Y,	DSP_POS_X + 90,	DSP_POS_Y + 14		 );

		dsp_control_init_done = 1;
	}

	if(!tsu.dsp_alive)
		GUI_SetColor(GUI_GRAY);
	else
		GUI_SetColor(GUI_WHITE);

	// Print text
	GUI_SetFont(&GUI_Font8x16_1);
	GUI_DispStringAt("DSP",	DSP_POS_X + 52,	DSP_POS_Y + 1);

	// No blinking
	if(!tsu.dsp_alive)
		return;

	// Create blinker
	if(tsu.dsp_blinker)
		GUI_SetColor(GUI_RED);
	else
		GUI_SetColor(GUI_BLACK);

	GUI_FillRoundedRect(	DSP_POS_X + 7,	DSP_POS_Y + 4,	DSP_POS_X + 26,	DSP_POS_Y + 10,		2);
}


//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_dsp_stat_init(void)
{
	// Moved to s-meter control
	//char   	buff[100];
	//if(tsu.dsp_alive)
	//{
		// Debug print DSP firmware version
	//	GUI_SetColor(GUI_GRAY);
	//	GUI_SetFont(&GUI_Font8x16_1);
	//	sprintf(buff,"DSP v: %d.%d.%d.%d",tsu.dsp_rev1,tsu.dsp_rev2,tsu.dsp_rev3,tsu.dsp_rev4);
	//	GUI_DispStringAt(buff,520,40);
	//}

	dsp_control_init_done = 0;
	ui_controls_dsp_stat_print();
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_dsp_stat_quit(void)
{

}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_dsp_stat_touch(void)
{

}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_dsp_stat_refresh(void)
{
	ui_controls_dsp_stat_print();
}

#endif
