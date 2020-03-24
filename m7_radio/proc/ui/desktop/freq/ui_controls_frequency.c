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

#include "ui_controls_frequency.h"
#include "desktop\ui_controls_layout.h"

static void ui_controls_frequency_vfo_a_initial_paint(uchar is_init);
static void ui_controls_frequency_vfo_b_initial_paint(uchar is_init);

static void ui_controls_frequency_update_vfo_a(ulong freq);
static void ui_controls_frequency_update_vfo_b(ulong freq);

// ------------------------------------------------
// Frequency public
__IO DialFrequency 				df;
// ------------------------------------------------

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_frequency_vfo_a_initial_paint
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
static void ui_controls_frequency_vfo_a_initial_paint(uchar is_init)
{
	//printf("ui_controls_frequency_vfo_a_initial_paint\r\n");
	//printf("active vfo is %d\r\n",tsu.active_vfo);

	// Set virtual segments initial value (diff than zero!)
	df.vfo_a_scr_freq 	= 0;
	df.last_screen_step	= 0xFFFFFF99;
	//
	df.dial_100_mhz		= 9;
	df.dial_010_mhz		= 9;
	df.dial_001_mhz		= 9;
	df.dial_100_khz		= 9;
	df.dial_010_khz		= 9;
	df.dial_001_khz		= 9;
	df.dial_100_hz		= 9;
	df.dial_010_hz		= 9;
	df.dial_001_hz		= 9;

	// Filler
	//GUI_SetColor(GUI_GRAY);
	GUI_SetColor(GUI_BLACK);
	GUI_FillRoundedRect((M_FREQ_X + 0),(M_FREQ_Y + 0),(M_FREQ_X + 267),(M_FREQ_Y + 40),2);

	// Frame
	#ifdef FRAME_MAIN_DIAL
	GUI_SetColor(GUI_DARKGRAY);
	//GUI_SetColor(GUI_WHITE);
	GUI_DrawRoundedRect((M_FREQ_X + 0),(M_FREQ_Y + 0),(M_FREQ_X + 267),(M_FREQ_Y + 40),2);
	#endif

	if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
		GUI_SetColor(GUI_LIGHTGREEN);
	else
		GUI_SetColor(GUI_GRAY);
	GUI_SetFont(&GUI_Font20B_1);
	GUI_DispStringAt("VFO A",(M_FREQ_X + FREQ_FONT_SIZE_X*4 + 5),(M_FREQ_Y - 27));

	// Digits colour
	GUI_SetFont(FREQ_FONT);
	if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
		GUI_SetColor(GUI_WHITE);
	else
		GUI_SetColor(GUI_GRAY);

	// Digits text
	if(is_init)
		GUI_DispStringAt("999.999.999",(M_FREQ_X + 1),(M_FREQ_Y + 5));
	else
		GUI_DispStringAt("___.___.___",(M_FREQ_X + 1),(M_FREQ_Y + 5));

	// Update frequency, but only if not active
	if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		ui_controls_frequency_update_vfo_a(tsu.band[tsu.curr_band].vfo_a);
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_frequency_vfo_b_initial_paint
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
static void ui_controls_frequency_vfo_b_initial_paint(uchar is_init)
{
	//printf("ui_controls_frequency_vfo_b_initial_paint\r\n");
	//printf("active vfo is %d\r\n",tsu.active_vfo);

	// Publics reset
	df.vfo_b_scr_freq 	= 0;
	//
	df.sdial_100_mhz	= 9;
	df.sdial_010_mhz	= 9;
	df.sdial_001_mhz	= 9;
	df.sdial_100_khz	= 9;
	df.sdial_010_khz	= 9;
	df.sdial_001_khz	= 9;
	df.sdial_100_hz		= 9;
	df.sdial_010_hz		= 9;
	df.sdial_001_hz		= 9;

	// Frame
	GUI_SetColor(GUI_GRAY);
	GUI_FillRoundedRect((M_FREQ1_X + 46),(M_FREQ1_Y + 0),(M_FREQ1_X + 170),(M_FREQ1_Y + 24),2);
	if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		GUI_SetColor(GUI_WHITE);
	else
		GUI_SetColor(GUI_LIGHTGRAY);
	GUI_DrawRoundedRect((M_FREQ1_X + 0),(M_FREQ1_Y + 0),(M_FREQ1_X + 170),(M_FREQ1_Y + 24),2);

	// Leading text background
	if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		GUI_SetColor(GUI_RED);
	else
		GUI_SetColor(GUI_LIGHTGRAY);
	GUI_FillRoundedRect((M_FREQ1_X + 1),(M_FREQ1_Y + 1),(M_FREQ1_X + 46),(M_FREQ1_Y + 23),2);

	// Leading text colour
	if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		GUI_SetColor(GUI_WHITE);
	else
		GUI_SetColor(GUI_GRAY);

	// Leading text
	GUI_SetFont(GUI_FONT_16B_1);
	GUI_DispStringAt("VFO B",(M_FREQ1_X + 3),(M_FREQ1_Y + 5));

	// Digits
	if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		GUI_SetColor(VFO_B_SEG_ON_COLOR);
	else
		GUI_SetColor(VFO_B_SEG_OFF_COLOR);

	GUI_SetFont(GUI_FONT_24B_1);
	GUI_DispStringAt("999.999.999",(M_FREQ1_X + 48),(M_FREQ1_Y + 1));

	// Update frequency
	if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
		ui_controls_frequency_update_vfo_b(tsu.band[tsu.curr_band].vfo_b);
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_frequency_init
//* Object              : Reset locals
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_frequency_init(void)
{
	// Initial values
	df.last_active_vfo 			= tsu.band[tsu.curr_band].active_vfo;
	df.vfo_a_segments_invalid 	= 1;
	df.vfo_b_segments_invalid 	= 1;

	// Create controls
	ui_controls_frequency_vfo_a_initial_paint(1);
	ui_controls_frequency_vfo_b_initial_paint(1);
}

void ui_controls_frequency_quit(void)
{
	//
}

void ui_controls_frequency_touch(void)
{
	//
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_frequency_update_vfo_a
//* Object              :
//* Notes    			: 1. this function needs to be really, really fast
//* Notes   			: 2. do not use too much stack, as callers chain could
//* Notes    			:    be long(all screen coordinates as const !!)
//* Notes    			: 3. no C lib calls whatsoever
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
static void ui_controls_frequency_update_vfo_a(ulong freq)
{
	uchar		d_100mhz,d_10mhz,d_1mhz;
	uchar		d_100khz,d_10khz,d_1khz;
	uchar		d_100hz,d_10hz,d_1hz;
	char		digit[2];

	// As this function would be called very often(lag bigger than 10mS is
	// visible on the frequency display), we need to make sure no needless
	// updates. So only when there is actual change of the frequency
	if(df.vfo_a_scr_freq == freq)
		return;

	//printf("freq a: %d\r\n",freq);

	// Did step change ? Then do full repaint
	// Still not perfect, as highlighted digit
	// changes only when dial is moved
	if(df.last_screen_step != tsu.step)
	{
		// Invalidate all
		df.vfo_a_segments_invalid = 1;

		// Save to prevent needless repaint
		df.last_screen_step = tsu.step;
	}

	// Terminate
	digit[1] = 0;

	// Set Digit font
	GUI_SetFont(FREQ_FONT);

	// -----------------------
	// See if 100 Mhz needs update
	d_100mhz = (freq/100000000);
	if((d_100mhz != df.dial_100_mhz) || (df.vfo_a_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_BLUE);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect((M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 0)),(M_FREQ_Y + 5),(M_FREQ_X + FREQ_FONT_SIZE_X + 1 + (FREQ_FONT_SIZE_X * 0)),(M_FREQ_Y + 5 + FREQ_FONT_SIZE_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
			GUI_SetColor(GUI_WHITE);
		else
			GUI_SetColor(GUI_GRAY);
		// To string
		digit[0] = 0x30 + (d_100mhz & 0x0F);
		// Update segment
		if(d_100mhz) GUI_DispStringAt(digit,(M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 0)),(M_FREQ_Y + 5));
		// Save value
		df.dial_100_mhz = d_100mhz;

	}
	// -----------------------
	// See if 10 Mhz needs update
	d_10mhz = (freq%100000000)/10000000;
	if((d_10mhz != df.dial_010_mhz) || (df.vfo_a_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_YELLOW);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect((M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 1)),(M_FREQ_Y + 5),(M_FREQ_X + FREQ_FONT_SIZE_X + 1 + (FREQ_FONT_SIZE_X * 1)),(M_FREQ_Y + 5 + FREQ_FONT_SIZE_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
		{
			if(tsu.step == T_STEP_10MHZ)
				GUI_SetColor(GUI_LIGHTGRAY);
			else
				GUI_SetColor(GUI_WHITE);
		}
		else
			GUI_SetColor(GUI_GRAY);
		// To string
		digit[0] = 0x30 + (d_10mhz & 0x0F);
		// Update segment
		if(d_100mhz)	// update if 100 MHz digit is being displayed
			GUI_DispStringAt(digit,(M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 1)),(M_FREQ_Y + 5));
		else
		{
			if(d_10mhz)
				GUI_DispStringAt(digit,(M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 1)),(M_FREQ_Y + 5));
		}
		// Save value
		df.dial_010_mhz = d_10mhz;
	}
	// -----------------------
	// See if 1 Mhz needs update
	d_1mhz = (freq%10000000)/1000000;
	if((d_1mhz != df.dial_001_mhz) || (df.vfo_a_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_BLUE);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect((M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 2)),(M_FREQ_Y + 5),(M_FREQ_X + FREQ_FONT_SIZE_X + 1 + (FREQ_FONT_SIZE_X * 2)),(M_FREQ_Y + 5 + FREQ_FONT_SIZE_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
		{
			if(tsu.step == T_STEP_1MHZ)
				GUI_SetColor(GUI_LIGHTGRAY);
			else
				GUI_SetColor(GUI_WHITE);
		}
		else
			GUI_SetColor(GUI_GRAY);
		// To string
		digit[0] = 0x30 + (d_1mhz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 2)),(M_FREQ_Y + 5));
		// Save value
		df.dial_001_mhz = d_1mhz;
	}
	// -----------------------
	// See if 100 khz needs update
	d_100khz = (freq%1000000)/100000;
	if((d_100khz != df.dial_100_khz) || (df.vfo_a_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_YELLOW);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect((M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 4)),(M_FREQ_Y + 5),(M_FREQ_X + FREQ_FONT_SIZE_X + 1 + (FREQ_FONT_SIZE_X * 4)),(M_FREQ_Y + 5 + FREQ_FONT_SIZE_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
		{
			if(tsu.step == T_STEP_100KHZ)
				GUI_SetColor(GUI_LIGHTGRAY);
			else
				GUI_SetColor(GUI_WHITE);
		}
		else
			GUI_SetColor(GUI_GRAY);
		// To string
		digit[0] = 0x30 + (d_100khz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 4)),(M_FREQ_Y + 5));
		// Save value
		df.dial_100_khz = d_100khz;
	}
	// -----------------------
	// See if 10 khz needs update
	d_10khz = (freq%100000)/10000;
	if((d_10khz != df.dial_010_khz) || (df.vfo_a_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_BLUE);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect((M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 5)),(M_FREQ_Y + 5),(M_FREQ_X + FREQ_FONT_SIZE_X + 1 + (FREQ_FONT_SIZE_X * 5)),(M_FREQ_Y + 5 + FREQ_FONT_SIZE_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
		{
			if(tsu.step == T_STEP_10KHZ)
				GUI_SetColor(GUI_LIGHTGRAY);
			else
				GUI_SetColor(GUI_WHITE);
		}
		else
			GUI_SetColor(GUI_GRAY);
		// To string
		digit[0] = 0x30 + (d_10khz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 5)),(M_FREQ_Y + 5));
		// Save value
		df.dial_010_khz = d_10khz;
	}
	// -----------------------
	// See if 1 khz needs update
	d_1khz = (freq%10000)/1000;
	if((d_1khz != df.dial_001_khz) || (df.vfo_a_segments_invalid))
	{
		//printf("df.dial_001_khz    : %08x\r\n",df.dial_001_khz);
		//printf("tsu.step    : %08x\r\n",tsu.step);

		// Delete segment
		//GUI_SetColor(GUI_YELLOW);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect((M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 6)),(M_FREQ_Y + 5),(M_FREQ_X + FREQ_FONT_SIZE_X + 1 + (FREQ_FONT_SIZE_X * 6)),(M_FREQ_Y + 5 + FREQ_FONT_SIZE_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
		{
			if(tsu.step == T_STEP_1KHZ)
				GUI_SetColor(GUI_LIGHTGRAY);
			else
				GUI_SetColor(GUI_WHITE);
		}
		else
			GUI_SetColor(GUI_GRAY);
		// To string
		digit[0] = 0x30 + (d_1khz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 6)),(M_FREQ_Y + 5));
		// Save value
		df.dial_001_khz = d_1khz;
	}
	// -----------------------
	// See if 100 hz needs update
	d_100hz = (freq%1000)/100;
	if((d_100hz != df.dial_100_hz) || (df.vfo_a_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_BLUE);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect((M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 8)),(M_FREQ_Y + 5),(M_FREQ_X + FREQ_FONT_SIZE_X + 1 + (FREQ_FONT_SIZE_X * 8)),(M_FREQ_Y + 5 + FREQ_FONT_SIZE_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
		{
			if(tsu.step == T_STEP_100HZ)
				GUI_SetColor(GUI_LIGHTGRAY);
			else
				GUI_SetColor(GUI_WHITE);
		}
		else
			GUI_SetColor(GUI_GRAY);
		// To string
		digit[0] = 0x30 + (d_100hz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 8)),(M_FREQ_Y + 5));
		// Save value
		df.dial_100_hz = d_100hz;
	}
	// -----------------------
	// See if 10 hz needs update
	d_10hz = (freq%100)/10;
	if((d_10hz != df.dial_010_hz) || (df.vfo_a_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_YELLOW);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect((M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 9)),(M_FREQ_Y + 5),(M_FREQ_X + FREQ_FONT_SIZE_X + 1 + (FREQ_FONT_SIZE_X * 9)),(M_FREQ_Y + 5 + FREQ_FONT_SIZE_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
		{
			if(tsu.step == T_STEP_10HZ)
				GUI_SetColor(GUI_LIGHTGRAY);
			else
				GUI_SetColor(GUI_WHITE);
		}
		else
			GUI_SetColor(GUI_GRAY);
		// To string
		digit[0] = 0x30 + (d_10hz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 9)),(M_FREQ_Y + 5));
		// Save value
		df.dial_010_hz = d_10hz;
	}
	// -----------------------
	// See if 1 hz needs update
	d_1hz = (freq%10)/1;
	if((d_1hz != df.dial_001_hz) || (df.vfo_a_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_BLUE);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect((M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 10)),(M_FREQ_Y + 5),(M_FREQ_X + FREQ_FONT_SIZE_X + 1 + (FREQ_FONT_SIZE_X * 10)),(M_FREQ_Y + 5 + FREQ_FONT_SIZE_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
		{
			if(tsu.step == T_STEP_1HZ)
				GUI_SetColor(GUI_LIGHTGRAY);
			else
				GUI_SetColor(GUI_WHITE);
		}
		else
			GUI_SetColor(GUI_GRAY);
		// To string
		digit[0] = 0x30 + (d_1hz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ_X + 1 + (FREQ_FONT_SIZE_X * 10)),(M_FREQ_Y + 5));
		// Save value
		df.dial_001_hz = d_1hz;
	}

	// Invalidation valid only for one call
	if(df.vfo_a_segments_invalid)
		df.vfo_a_segments_invalid 	= 0;

	// Save to public, to prevent stalling the task
	df.vfo_a_scr_freq = freq;
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_frequency_update_vfo_b
//* Object              :
//* Notes    			: 1. this function needs to be really, really fast
//* Notes   			: 2. do not use too much stack, as callers chain could
//* Notes    			:    be long(all screen coordinates as const !!)
//* Notes    			: 3. no C lib calls whatsoever
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
static void ui_controls_frequency_update_vfo_b(ulong freq)
{
	uchar		d_100mhz,d_10mhz,d_1mhz;
	uchar		d_100khz,d_10khz,d_1khz;
	uchar		d_100hz,d_10hz,d_1hz;
	char		digit[2];

	// As this function would be called very often(lag bigger than 10mS is
	// visible on the frequency display), we need to make sure no needless
	// updates. So only when there is actual change of the frequency
	if(df.vfo_b_scr_freq == freq)
		return;

	//printf("freq b: %d\r\n",freq);

	// Did step change ? Then do full repaint
	// by resetting saved segment values
	// Still not perfect, as highlighted digit
	// changes only when dial is moved
	if(df.last_screen_step != tsu.step)
	{
		// Invalidate all
		df.vfo_b_segments_invalid = 1;

		// Save to prevent needless repaint
		df.last_screen_step = tsu.step;
	}

	// Terminate
	digit[1] = 0;

	// Set Digit font
	GUI_SetFont(GUI_FONT_24B_1);

	// -----------------------
	// See if 100 Mhz needs update
	d_100mhz = (freq/100000000);
	if((d_100mhz != df.sdial_100_mhz) || (df.vfo_b_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_BLUE);
		GUI_SetColor(GUI_GRAY);
		GUI_FillRect((M_FREQ1_X + 47 + (FREQ_FONT_SIZE1_X * 0)),(M_FREQ1_Y + 4),(M_FREQ1_X + 46 + (FREQ_FONT_SIZE1_X * 1)),(M_FREQ1_Y + 4 + FREQ_FONT_SIZE1_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
			GUI_SetColor(VFO_B_SEG_ON_COLOR);
		else
			GUI_SetColor(VFO_B_SEG_OFF_COLOR);
		// To string
		digit[0] = 0x30 + (d_100mhz & 0x0F);
		// Update segment
		if(d_100mhz) GUI_DispStringAt(digit,(M_FREQ1_X + 48 + (FREQ_FONT_SIZE1_X * 0)),(M_FREQ1_Y + 1));
		// Save value
		df.sdial_100_mhz = d_100mhz;

	}
	// -----------------------
	// See if 10 Mhz needs update
	d_10mhz = (freq%100000000)/10000000;
	if((d_10mhz != df.sdial_010_mhz) || (df.vfo_b_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_YELLOW);
		GUI_SetColor(GUI_GRAY);
		GUI_FillRect((M_FREQ1_X + 47 + (FREQ_FONT_SIZE1_X * 1)),(M_FREQ1_Y + 4),(M_FREQ1_X + 46 + (FREQ_FONT_SIZE1_X * 2)),(M_FREQ1_Y + 4 + FREQ_FONT_SIZE1_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		{
			if(tsu.step == T_STEP_10MHZ)
				GUI_SetColor(VFO_B_SEG_SEL_COLOR);
			else
				GUI_SetColor(VFO_B_SEG_ON_COLOR);
		}
		else
			GUI_SetColor(VFO_B_SEG_OFF_COLOR);
		// To string
		digit[0] = 0x30 + (d_10mhz & 0x0F);
		// Update segment
		if(d_100mhz)	// update if 100 MHz digit is being displayed
			GUI_DispStringAt(digit,(M_FREQ1_X + 48 + (FREQ_FONT_SIZE1_X * 1)),(M_FREQ1_Y + 1));
		else
		{
			if(d_10mhz)
				GUI_DispStringAt(digit,(M_FREQ1_X + 48 + (FREQ_FONT_SIZE1_X * 1)),(M_FREQ1_Y + 1));
		}
		// Save value
		df.sdial_010_mhz = d_10mhz;
	}
	// -----------------------
	// See if 1 Mhz needs update
	d_1mhz = (freq%10000000)/1000000;
	if((d_1mhz != df.sdial_001_mhz) || (df.vfo_b_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_BLUE);
		GUI_SetColor(GUI_GRAY);
		GUI_FillRect((M_FREQ1_X + 47 + (FREQ_FONT_SIZE1_X * 2)),(M_FREQ1_Y + 4),(M_FREQ1_X + 46 + (FREQ_FONT_SIZE1_X * 3)),(M_FREQ1_Y + 4 + FREQ_FONT_SIZE1_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		{
			if(tsu.step == T_STEP_1MHZ)
				GUI_SetColor(VFO_B_SEG_SEL_COLOR);
			else
				GUI_SetColor(VFO_B_SEG_ON_COLOR);
		}
		else
			GUI_SetColor(VFO_B_SEG_OFF_COLOR);
		// To string
		digit[0] = 0x30 + (d_1mhz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ1_X + 48 + (FREQ_FONT_SIZE1_X * 2)),(M_FREQ1_Y + 1));
		// Save value
		df.sdial_001_mhz = d_1mhz;
	}
	// -----------------------
	// See if 100 khz needs update
	d_100khz = (freq%1000000)/100000;
	if((d_100khz != df.sdial_100_khz) || (df.vfo_b_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_YELLOW);
		GUI_SetColor(GUI_GRAY);
		GUI_FillRect((M_FREQ1_X + 41 + (FREQ_FONT_SIZE1_X * 4)),(M_FREQ1_Y + 4),(M_FREQ1_X + 40 + (FREQ_FONT_SIZE1_X * 5)),(M_FREQ1_Y + 4 + FREQ_FONT_SIZE1_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		{
			if(tsu.step == T_STEP_100KHZ)
				GUI_SetColor(VFO_B_SEG_SEL_COLOR);
			else
				GUI_SetColor(VFO_B_SEG_ON_COLOR);
		}
		else
			GUI_SetColor(VFO_B_SEG_OFF_COLOR);
		// To string
		digit[0] = 0x30 + (d_100khz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ1_X + 42 + (FREQ_FONT_SIZE1_X * 4)),(M_FREQ1_Y + 1));
		// Save value
		df.sdial_100_khz = d_100khz;
	}
	// -----------------------
	// See if 10 khz needs update
	d_10khz = (freq%100000)/10000;
	if((d_10khz != df.sdial_010_khz) || (df.vfo_b_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_BLUE);
		GUI_SetColor(GUI_GRAY);
		GUI_FillRect((M_FREQ1_X + 41 + (FREQ_FONT_SIZE1_X * 5)),(M_FREQ1_Y + 4),(M_FREQ1_X + 40 + (FREQ_FONT_SIZE1_X * 6)),(M_FREQ1_Y + 4 + FREQ_FONT_SIZE1_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		{
			if(tsu.step == T_STEP_10KHZ)
				GUI_SetColor(VFO_B_SEG_SEL_COLOR);
			else
				GUI_SetColor(VFO_B_SEG_ON_COLOR);
		}
		else
			GUI_SetColor(VFO_B_SEG_OFF_COLOR);
		// To string
		digit[0] = 0x30 + (d_10khz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ1_X + 42 + (FREQ_FONT_SIZE1_X * 5)),(M_FREQ1_Y + 1));
		// Save value
		df.sdial_010_khz = d_10khz;
	}
	// -----------------------
	// See if 1 khz needs update
	d_1khz = (freq%10000)/1000;
	if((d_1khz != df.sdial_001_khz) || (df.vfo_b_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_YELLOW);
		GUI_SetColor(GUI_GRAY);
		GUI_FillRect((M_FREQ1_X + 41 + (FREQ_FONT_SIZE1_X * 6)),(M_FREQ1_Y + 4),(M_FREQ1_X + 40 + (FREQ_FONT_SIZE1_X * 7)),(M_FREQ1_Y + 4 + FREQ_FONT_SIZE1_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		{
			if(tsu.step == T_STEP_1KHZ)
				GUI_SetColor(VFO_B_SEG_SEL_COLOR);
			else
				GUI_SetColor(VFO_B_SEG_ON_COLOR);
		}
		else
			GUI_SetColor(VFO_B_SEG_OFF_COLOR);
		// To string
		digit[0] = 0x30 + (d_1khz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ1_X + 42 + (FREQ_FONT_SIZE1_X * 6)),(M_FREQ1_Y + 1));
		// Save value
		df.sdial_001_khz = d_1khz;
	}
	// -----------------------
	// See if 100 hz needs update
	d_100hz = (freq%1000)/100;
	if((d_100hz != df.sdial_100_hz) || (df.vfo_b_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_BLUE);
		GUI_SetColor(GUI_GRAY);
		GUI_FillRect((M_FREQ1_X + 35 + (FREQ_FONT_SIZE1_X * 8)),(M_FREQ1_Y + 4),(M_FREQ1_X + 34 + (FREQ_FONT_SIZE1_X * 9)),(M_FREQ1_Y + 4 + FREQ_FONT_SIZE1_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		{
			if(tsu.step == T_STEP_100HZ)
				GUI_SetColor(VFO_B_SEG_SEL_COLOR);
			else
				GUI_SetColor(VFO_B_SEG_ON_COLOR);
		}
		else
			GUI_SetColor(VFO_B_SEG_OFF_COLOR);
		// To string
		digit[0] = 0x30 + (d_100hz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ1_X + 36 + (FREQ_FONT_SIZE1_X * 8)),(M_FREQ1_Y + 1));
		// Save value
		df.sdial_100_hz = d_100hz;
	}
	// -----------------------
	// See if 10 hz needs update
	d_10hz = (freq%100)/10;
	if((d_10hz != df.sdial_010_hz) || (df.vfo_b_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_YELLOW);
		GUI_SetColor(GUI_GRAY);
		GUI_FillRect((M_FREQ1_X + 35 + (FREQ_FONT_SIZE1_X * 9)),(M_FREQ1_Y + 4),(M_FREQ1_X + 34 + (FREQ_FONT_SIZE1_X * 10)),(M_FREQ1_Y + 4 + FREQ_FONT_SIZE1_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		{
			if(tsu.step == T_STEP_10HZ)
				GUI_SetColor(VFO_B_SEG_SEL_COLOR);
			else
				GUI_SetColor(VFO_B_SEG_ON_COLOR);
		}
		else
			GUI_SetColor(VFO_B_SEG_OFF_COLOR);
		// To string
		digit[0] = 0x30 + (d_10hz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ1_X + 36 + (FREQ_FONT_SIZE1_X * 9)),(M_FREQ1_Y + 1));
		// Save value
		df.sdial_010_hz = d_10hz;
	}
	// -----------------------
	// See if 1 hz needs update
	d_1hz = (freq%10)/1;
	if((d_1hz != df.sdial_001_hz) || (df.vfo_b_segments_invalid))
	{
		// Delete segment
		//GUI_SetColor(GUI_BLUE);
		GUI_SetColor(GUI_GRAY);
		GUI_FillRect((M_FREQ1_X + 35 + (FREQ_FONT_SIZE1_X * 10)),(M_FREQ1_Y + 4),(M_FREQ1_X + 34 + (FREQ_FONT_SIZE1_X * 11)),(M_FREQ1_Y + 4 + FREQ_FONT_SIZE1_Y));
		if(tsu.band[tsu.curr_band].active_vfo == VFO_B)
		{
			if(tsu.step == T_STEP_1HZ)
				GUI_SetColor(VFO_B_SEG_SEL_COLOR);
			else
				GUI_SetColor(VFO_B_SEG_ON_COLOR);
		}
		else
			GUI_SetColor(VFO_B_SEG_OFF_COLOR);
		// To string
		digit[0] = 0x30 + (d_1hz & 0x0F);
		// Update segment
		GUI_DispStringAt(digit,(M_FREQ1_X + 36 + (FREQ_FONT_SIZE1_X * 10)),(M_FREQ1_Y + 1));
		// Save value
		df.sdial_001_hz = d_1hz;
	}

	// Invalidation valid only for one call
	if(df.vfo_b_segments_invalid)
		df.vfo_b_segments_invalid 	= 0;

	// Save to public, to prevent stalling the task
	df.vfo_b_scr_freq = freq;
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_frequency_refresh
//* Object              :
//* Notes    			:
//* Notes    			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_frequency_refresh(uchar flags)
{
	ulong vfo_af;

	// Did the active VFO changed since last repaint ?
	if(df.last_active_vfo != tsu.band[tsu.curr_band].active_vfo)
	{
		//printf("vfo changed\r\n");

		// ToDo: If switching from center to fixed mode, copy VFO A to VFO B
		// 		 and on band changes, etc...


		// -----------------------------------------------------------
		// Update main oscillator frequency on vfo change
		tsu.update_freq_dsp_req = 1;

		ui_controls_frequency_vfo_a_initial_paint(0);
		ui_controls_frequency_vfo_b_initial_paint(0);

		// Save
		df.last_active_vfo = tsu.band[tsu.curr_band].active_vfo;

		return;
	}

	// Normal dial repaint
	if(tsu.band[tsu.curr_band].active_vfo == VFO_A)
	{
		vfo_af = tsu.band[tsu.curr_band].vfo_a;		//tsu.vfo_a;

		// On screen frequency = (Osc + NCO)
		if(tsu.band[tsu.curr_band].fixed_mode)
			vfo_af += tsu.band[tsu.curr_band].nco_freq;

		ui_controls_frequency_update_vfo_a(vfo_af);
	}
	else
	{
		ui_controls_frequency_update_vfo_b(tsu.band[tsu.curr_band].vfo_b);
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_frequency_change_band
//* Object              : call from band control
//* Notes    			: - full repaint on both VFOs
//* Notes    			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_frequency_change_band(void)
{
	ui_controls_frequency_vfo_a_initial_paint(0);
	ui_controls_frequency_vfo_b_initial_paint(0);

	ui_controls_frequency_update_vfo_a(tsu.band[tsu.curr_band].vfo_a);
	ui_controls_frequency_update_vfo_b(tsu.band[tsu.curr_band].vfo_b);
}

#endif
