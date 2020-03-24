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

#include "gui.h"
#include "dialog.h"
//#include "ST_GUI_Addons.h"

#include "ui_controls_agc.h"
#include "desktop\ui_controls_layout.h"
#include "hw\dsp_eep\hw_dsp_eep.h"

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
static void ui_controls_repaint_state(void)
{
	char  		buff[20];

	// Clear AGC gain part of control
	GUI_SetColor(GUI_WHITE);
	GUI_FillRoundedRect((AGC_X + 43),(AGC_Y + 2),(AGC_X + 100),(AGC_Y + 17),2);
	GUI_SetColor(GUI_GRAY);
	GUI_SetFont(&GUI_Font20_1);

	// Display AGC state
	switch(tsu.agc_state)
	{
		case AGC_SLOW:
			GUI_DispStringAt("SLOW",	(AGC_X + 45),(AGC_Y + 0));
			break;
		case AGC_MED:
			GUI_DispStringAt("MED",		(AGC_X + 54),(AGC_Y + 0));
			break;
		case AGC_FAST:
			GUI_DispStringAt("FAST",	(AGC_X + 50),(AGC_Y + 0));
			break;
		case AGC_CUSTOM:
			GUI_DispStringAt("CUST",	(AGC_X + 47),(AGC_Y + 0));
			break;
		case AGC_OFF:
			GUI_DispStringAt("OFF",		(AGC_X + 55),(AGC_Y + 0));
			break;
		default:
			break;
	}

	//EnterCriticalSection();
	sprintf(buff,"%2d",tsu.rf_gain);
	//ExitCriticalSection();

	// Clean area first ?
	// ...

	// Display RF gain
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt(buff,(AGC_X + 105),(AGC_Y + 0));
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_agc_init(void)
{
	// Create control
	GUI_SetColor(GUI_GRAY);
	GUI_FillRoundedRect((AGC_X + 0),(AGC_Y + 0),(AGC_X + 128),(AGC_Y + 19),2);
	GUI_SetFont(&GUI_Font20_1);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt("AGC",(AGC_X + 2),(AGC_Y + 0));

	// Initial paint
	ui_controls_repaint_state();
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_agc_quit(void)
{
	//
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_agc_touch(void)
{
	// control is too small for touch input
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_agc_refresh(void)
{
	// nothing here as update is done in the side encoder options menu
}
