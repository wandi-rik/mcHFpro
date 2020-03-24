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

#include "ui_controls_keyer.h"
#include "desktop\ui_controls_layout.h"

#include "GUI.h"

// ------------------------------
//#include "touch_driver.h"
//extern struct TD 		t_d;
// ------------------------------

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

uchar	local_dah_press = 0;
uchar	local_dit_press = 0;

static void ui_controls_keyer_draw_top(uchar press)
{
	// Button background
	if(press)
		GUI_SetColor(GUI_WHITE);
	else
		GUI_SetColor(GUI_GRAY);

	GUI_FillRoundedFrame(	IAMB_BTN_TOP_X0,
							IAMB_BTN_TOP_Y0,
							IAMB_BTN_TOP_X1,
							IAMB_BTN_TOP_Y1,
							SW_FRAME_CORNER_R,
							SW_FRAME_WIDTH
						);
	// Dah symbol
	if(press)
		GUI_SetColor(GUI_GRAY);
	else
		GUI_SetColor(GUI_WHITE);

	GUI_FillRoundedFrame(	(IAMB_BTN_TOP_X0 + IAMB_KEYER_SIZE_X/2 - 25),
							(IAMB_BTN_TOP_Y0 + IAMB_KEYER_SIZE_Y/4 - 10),
							(IAMB_BTN_TOP_X0 + IAMB_KEYER_SIZE_X/2 + 25),
							(IAMB_BTN_TOP_Y0 + IAMB_KEYER_SIZE_Y/4 +  6),
							8,
							1
						);
}

static void ui_controls_keyer_draw_btm(uchar press)
{
	// Button background
	if(press)
		GUI_SetColor(GUI_WHITE);
	else
		GUI_SetColor(GUI_GRAY);

	GUI_FillRoundedFrame(	IAMB_BTN_BTM_X0,
							IAMB_BTN_BTM_Y0,
							IAMB_BTN_BTM_X1,
							IAMB_BTN_BTM_Y1,
							SW_FRAME_CORNER_R,
							SW_FRAME_WIDTH
						);
	// Dit symbol
	if(press)
		GUI_SetColor(GUI_GRAY);
	else
		GUI_SetColor(GUI_WHITE);

	GUI_FillRoundedFrame(	(IAMB_BTN_BTM_X0 + IAMB_KEYER_SIZE_X/2 -  8),
							(IAMB_BTN_BTM_Y0 + IAMB_KEYER_SIZE_Y/4 - 10),
							(IAMB_BTN_BTM_X0 + IAMB_KEYER_SIZE_X/2 +  8),
							(IAMB_BTN_BTM_Y0 + IAMB_KEYER_SIZE_Y/4 +  6),
							6,
							1
						);

}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_keyer_init
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_keyer_init(void)
{
	// Draw control frame
	GUI_SetColor(GUI_ORANGE);
	GUI_DrawRoundedFrame(	IAMB_KEYER_X,
							IAMB_KEYER_Y,
							(IAMB_KEYER_X + IAMB_KEYER_SIZE_X),
							(IAMB_KEYER_Y + IAMB_KEYER_SIZE_Y),
							SW_FRAME_CORNER_R,
							SW_FRAME_WIDTH
						);

	// Draw separator
	GUI_DrawHLine((IAMB_KEYER_Y + IAMB_KEYER_SIZE_Y/2 - 1),(IAMB_KEYER_X + 0),(IAMB_KEYER_X + IAMB_KEYER_SIZE_X));
	GUI_DrawHLine((IAMB_KEYER_Y + IAMB_KEYER_SIZE_Y/2 + 0),(IAMB_KEYER_X + 0),(IAMB_KEYER_X + IAMB_KEYER_SIZE_X));
	GUI_DrawHLine((IAMB_KEYER_Y + IAMB_KEYER_SIZE_Y/2 + 1),(IAMB_KEYER_X + 0),(IAMB_KEYER_X + IAMB_KEYER_SIZE_X));

	// Draw buttons
	ui_controls_keyer_draw_top(0);
	ui_controls_keyer_draw_btm(0);
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_keyer_quit(void)
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
void ui_controls_keyer_touch(void)
{
#if 0
	//printf("keyer touch\r\n");

	// Only in CW mode and if displayed
	if(tsu.band[tsu.curr_band].demod_mode != DEMOD_CW) //&& (ui_sw.ctrl_type == SW_CONTROL_MID))
		return;

	// Top button
	if(		(t_d.point_x[0] >= IAMB_BTN_TOP_X0) &&\
			(t_d.point_x[0] <= IAMB_BTN_TOP_X1) &&\
			(t_d.point_y[0] >= IAMB_BTN_TOP_Y0) &&\
			(t_d.point_y[0] <= IAMB_BTN_TOP_Y1)
	  )
	{
		//printf("------------------------\r\n");
		//printf("top touched\r\n");

		// Paint as active
		local_dah_press = 1;
		ui_controls_keyer_draw_top(1);

		// Deactivate the other
		if(local_dit_press) ui_controls_keyer_draw_btm(0);

		// Pass message to API driver
		tsu.cw_iamb_type 	= 2;							// Set keyer button
		if(tsu.cw_tx_state == 0) tsu.cw_tx_state 	= 1;	// Set state
		return;
	}

	// Bottom button
	if(		(t_d.point_x[0] >= IAMB_BTN_BTM_X0) &&\
			(t_d.point_x[0] <= IAMB_BTN_BTM_X1) &&\
			(t_d.point_y[0] >= IAMB_BTN_BTM_Y0) &&\
			(t_d.point_y[0] <= IAMB_BTN_BTM_Y1)
	  )
	{
		//printf("------------------------\r\n");
		//printf("bottom touched\r\n");

		// Paint as active
		local_dit_press = 1;
		ui_controls_keyer_draw_btm(1);

		// Deactivate the other
		if(local_dah_press) ui_controls_keyer_draw_top(0);

		// Pass message to API driver
		tsu.cw_iamb_type 	= 1;							// Set keyer button
		if(tsu.cw_tx_state == 0) tsu.cw_tx_state = 1;		// Set state
		return;
	}
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_keyer_refresh(void)
{
	//printf("refresh->cw_tx_state = %d\r\n",tsu.cw_tx_state);
	//printf("refresh->t_d.pending = %d\r\n",t_d.pending);
#if 0
	// Button was pressed
	if((!t_d.pending) && (tsu.cw_tx_state == 1))
	//if((!t_d.pending) && (local_dah_press || local_dit_press))
	{
		//printf("keyer reset\r\n");

		tsu.cw_iamb_type 	= 0;
		tsu.cw_tx_state 	= 2;

		// Reset
		if(local_dah_press) ui_controls_keyer_draw_top(0);
		if(local_dit_press) ui_controls_keyer_draw_btm(0);
	}
#endif
}

#endif
