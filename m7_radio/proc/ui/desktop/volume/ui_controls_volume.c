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

#include "ui_controls_volume.h"
#include "desktop\ui_controls_layout.h"

#include "stm32h7xx_hal_gpio.h"

// Speaker icon in C file as binary resource
extern GUI_CONST_STORAGE GUI_BITMAP bmtechrubio;
extern GUI_CONST_STORAGE GUI_BITMAP bmtechrubio_mute;

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

// control flags
uchar loc_volume 		= 0;
uchar mute_flag	 		= 0;
uchar mute_saved_vol 	= 0;
uchar mute_debounce 	= 0;

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_volume_init(void)
{
	char  		buff[20];
	GUI_RECT 	Rect;

	if(!mute_flag)
		GUI_DrawBitmap(&bmtechrubio, (SPEAKER_X + 1), (SPEAKER_Y + 1));
	else
		GUI_DrawBitmap(&bmtechrubio_mute, (SPEAKER_X + 1), (SPEAKER_Y + 1));

	GUI_SetColor(GUI_YELLOW);
	GUI_DrawRoundedRect((SPEAKER_X + 0),(SPEAKER_Y + 0),(SPEAKER_X + 61),(SPEAKER_Y + 49),2);
	GUI_DrawRoundedRect((SPEAKER_X + 1),(SPEAKER_Y + 1),(SPEAKER_X + 60),(SPEAKER_Y + 48),2);

	if(!mute_flag)
	{
		EnterCriticalSection();
		sprintf(buff,"%2d",tsu.band[tsu.curr_band].volume);
		ExitCriticalSection();
	}
	else
	{
		EnterCriticalSection();
		sprintf(buff,"%2d",mute_saved_vol);
		ExitCriticalSection();
	}

#if 0
	// Clear area
	GUI_SetColor(GUI_WHITE);
	GUI_FillRect((SPEAKER_X + 2),(SPEAKER_Y + 37),(SPEAKER_X + 18),(SPEAKER_Y + 46));

	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font8x8_ASCII);
	GUI_DispStringAt(buff,(SPEAKER_X + 2),(SPEAKER_Y + 38));
#endif

	Rect.x0 = (SPEAKER_X + 2);
	Rect.y0 = (SPEAKER_Y + 37);
	Rect.x1 = (SPEAKER_X + 18);
	Rect.y1 = (SPEAKER_Y + 46);

	GUI_SetColor(GUI_BLUE);
	GUI_SetFont(&GUI_Font8x8_ASCII);

	// Set a clip rectangle to save performance, otherwise the whole background would be redrawn
	GUI_SetClipRect(&Rect);

	GUI_DispStringInRect(buff, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

	GUI_SetClipRect(NULL);
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_volume_quit(void)
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
void ui_controls_volume_touch(void)
{
	//printf("volume touch\r\n");

	if(mute_debounce)
	{
		//printf("timer on\r\n");

		mute_debounce--;
		if(mute_debounce)
			return;
	}

	//printf("volume mute process...\r\n");

	// Flip flag
	mute_flag = !mute_flag;

	if(mute_flag)
	{
		// Save volume
		mute_saved_vol = tsu.band[tsu.curr_band].volume;

		// Mute
		tsu.band[tsu.curr_band].volume = 0;

		// Prevent local refresh
		loc_volume = 0;
	}
	else
	{
		// Restore volume
		tsu.band[tsu.curr_band].volume = mute_saved_vol;

		// Prevent local refresh
		loc_volume = mute_saved_vol;
	}

	// Pass request
	tsu.update_audio_dsp_req = 1;

	// Redraw
	ui_controls_volume_init();

	mute_debounce = 3;

	tsu.audio_mute_flag = mute_flag;
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_volume_refresh(void)
{
	uchar volume = tsu.band[tsu.curr_band].volume;
	char  buff[20];

	// Leave volume digit on screen while mute
	if(mute_flag)
		return;

	// Do not over update
	if(loc_volume == volume)
		return;

	// Set request flag
	tsu.update_audio_dsp_req = 1;

	// Clear area
	GUI_SetColor(GUI_WHITE);
	GUI_FillRect((SPEAKER_X + 2),(SPEAKER_Y + 37),(SPEAKER_X + 18),(SPEAKER_Y + 46));

	GUI_SetColor(GUI_BLUE);
	GUI_SetFont(&GUI_Font8x8_ASCII);

	EnterCriticalSection();
	sprintf(buff,"%2d",volume);
	ExitCriticalSection();

	GUI_DispStringAt(buff,(SPEAKER_X + 2),(SPEAKER_Y + 38));

	loc_volume = volume;
}

#endif
