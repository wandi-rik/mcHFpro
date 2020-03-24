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

#include "ui_controls_demod.h"
#include "desktop\ui_controls_layout.h"

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

uchar loc_demod_mode = 0;

static void ui_controls_demod_change_screen_demod_mode(uchar on_init)
{
	uchar demod = tsu.band[tsu.curr_band].demod_mode;

	if(!on_init)
	{
		if(loc_demod_mode == demod)
			return;
	}

	// DSP update request
	tsu.update_demod_dsp_req = 1;

	GUI_SetColor(GUI_BLUE);
	GUI_FillRoundedRect((DECODER_MODE_X + 2),(DECODER_MODE_Y + 2),(DECODER_MODE_X + 50),(DECODER_MODE_Y + 18),2);

	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_Font20_1);

	switch(demod)
	{
		case DEMOD_USB:
			GUI_DispStringAt("USB",(DECODER_MODE_X + 10),(DECODER_MODE_Y + 1));
			break;
		case DEMOD_LSB:
			GUI_DispStringAt("LSB",(DECODER_MODE_X + 10),(DECODER_MODE_Y + 1));
			break;
		case DEMOD_CW:
			GUI_DispStringAt("CW",(DECODER_MODE_X + 10),(DECODER_MODE_Y + 1));
			break;
		case DEMOD_AM:
			GUI_DispStringAt("AM",(DECODER_MODE_X + 10),(DECODER_MODE_Y + 1));
			break;
		//case DEMOD_DIGI:	- no point really, as we are going to repaint entirely different Desktop
		//	GUI_DispStringAt("FT8",(DECODER_MODE_X + 10),(DECODER_MODE_Y + 1));
		//	break;
		default:
			break;
	}

	loc_demod_mode = demod;

	//--WRITE_EEPROM(EEP_DEMOD_MOD,tsu.demod_mode);
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_demod_init
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_demod_init(void)
{
	GUI_SetColor(GUI_GRAY);
	GUI_DrawRoundedRect((DECODER_MODE_X + 0),(DECODER_MODE_Y + 0),(DECODER_MODE_X + 52),(DECODER_MODE_Y + 20),2);
	GUI_DrawRoundedRect((DECODER_MODE_X + 1),(DECODER_MODE_Y + 1),(DECODER_MODE_X + 51),(DECODER_MODE_Y + 19),2);
	GUI_SetColor(GUI_BLUE);
	GUI_FillRoundedRect((DECODER_MODE_X + 2),(DECODER_MODE_Y + 2),(DECODER_MODE_X + 50),(DECODER_MODE_Y + 18),2);
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_Font20_1);

	// Initial paint
	ui_controls_demod_change_screen_demod_mode(1);
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_demod_quit(void)
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
void ui_controls_demod_touch(void)
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
void ui_controls_demod_refresh(void)
{
	ui_controls_demod_change_screen_demod_mode(0);
}

#endif
