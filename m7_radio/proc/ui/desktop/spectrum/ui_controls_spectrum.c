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

//  Description:    controls are doubled as new functions, sacrificing code size   **
//  Description:    for speed of execution(less publics in RAM, mostly constants   **
//  Description:    as intermediate values for asm instructions)                   **

#include <math.h>
#include <stddef.h>

//#include "main.h"
#include "mchf_pro_board.h"

#ifdef CONTEXT_DRIVER_UI

#include "ui_proc.h"
#include "ui_controls_spectrum.h"
#include "desktop\ui_controls_layout.h"

// ------------------------------
//#include "touch_driver.h"
//extern struct TD 		t_d;
// ------------------------------

// Memory device created for the combined Spectrum/Waterfall
// control
GUI_MEMDEV_Handle hMemSpWf = 0;

const ulong waterfall_blue[65] =
{
    0x00002e,
    0x020231,
    0x050534,
    0x070837,
    0x0a0b3b,
    0x0c0d3e,
    0x0f1041,
    0x121345,
    0x141648,
    0x17194b,
    0x191b4f,
    0x1c1e52,
    0x1f2155,
    0x212459,
    0x24275c,
    0x26295f,
    0x292c63,
    0x2b2f66,
    0x2e3269,
    0x31356d,
    0x333770,
    0x363a73,
    0x383d76,
    0x3b407a,
    0x3e437d,
    0x404580,
    0x434884,
    0x454b87,
    0x484e8a,
    0x4b518e,
    0x4d5391,
    0x505694,
    0x525998,
    0x555c9b,
    0x575e9e,
    0x5a61a2,
    0x5d64a5,
    0x5f67a8,
    0x626aac,
    0x646caf,
    0x676fb2,
    0x6a72b6,
    0x6c75b9,
    0x6f78bc,
    0x717abf,
    0x747dc3,
    0x7780c6,
    0x7983c9,
    0x7c86cd,
    0x7e88d0,
    0x818dd3,
    0x838ed7,
    0x8691da,
    0x8994dd,
    0x8b96e1,
    0x8e99e4,
    0x909ce7,
    0x939feb,
    0x96a2ee,
    0x98a4f1,
    0x9ba7f5,
    0x9ddaf8,
    0xa0dffb,
    0xa3e5ff,
    0xffffff
};

static void ui_controls_create_header_big(void);

struct UI_SW	ui_sw;

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

// temp public
//unsigned char fft_value[1024];

// Default values
//int 		bandpass_start 	= 360;
//int 		bandpass_end   	= 400;

ushort		old_dif			= 0;

uchar 		loc_vfo_mode;
// api driver test
//extern uchar api_conv_type;

uchar 	api_conv_type 	= 1;						// smooth waterfall
uchar 	sw_light		= 1;						// simplified scope (less resources)

#if 0
// redundant!
static void ui_controls_spectrum_iambic_keyer(void)
{
	GUI_SetAlpha(168);
	GUI_SetColor(GUI_ORANGE);
	GUI_FillRoundedRect(SCOPE_X + 700, SCOPE_Y, SCOPE_X + SCOPE_X_SIZE - 10, SCOPE_Y + SCOPE_Y_SIZE - 10, 4);
	GUI_SetAlpha(0);
}
#endif
static void ui_controls_spectrum_fft_process_big(void)
{
	ulong i,j;
	uchar curr_val,prev_val,next_val,aver;

#if 1
	// Copy to fft buffer
	// but cut left and right 28 points then expand four times
	for(i = 28, j = 0; i < 227; i++)
	{
		ui_sw.fft_value[j + 0] = 0 + ui_sw.fft_dsp[i];
		ui_sw.fft_value[j + 1] = 0 + ui_sw.fft_dsp[i];
		ui_sw.fft_value[j + 2] = 0 + ui_sw.fft_dsp[i];
		ui_sw.fft_value[j + 3] = 0 + ui_sw.fft_dsp[i];

		j += 4;
	}
#endif

#if 1
	// only small chunk left
	for(i = 0; i < 256; i++)
		ui_sw.fft_value[i] = ui_sw.fft_dsp[i];
#endif

#if 1
	if(api_conv_type == 0)
	{
		// Copy to fft buffer
		// 796 width of spectrum, expand 256 fft to 768 points
		for(i = 0, j = 14; i < 256; i++)
		{
			ui_sw.fft_value[j + 0] = ui_sw.fft_dsp[i];
			ui_sw.fft_value[j + 1] = ui_sw.fft_dsp[i];
			ui_sw.fft_value[j + 2] = ui_sw.fft_dsp[i];

			j += 3;
		}
	}
	else
	{
		// Emulate data in the left corner, outside of fft values
		for(i = 0; i < 14; i++)
		{
			if(i%5)// ToDo: fix this - call to C lib!!
				ui_sw.fft_value[i] = ui_sw.fft_dsp[0];
			else
				ui_sw.fft_value[i] = ui_sw.fft_dsp[0] + 1;
		}

		// Copy to fft buffer
		// 796 width of spectrum, expand 256 fft to 768 points
		// use approximation for adjacent points (only mid one is real value)
		for(i = 0, j = 14; i < 256; i++)
		{
			// Get current point
			curr_val = ui_sw.fft_dsp[i];

			// Get previous point
			if(i == 0)
				prev_val = curr_val;
			else
				prev_val = ui_sw.fft_dsp[i - 1];

			// Get next point
			if(i < 254)
				next_val = ui_sw.fft_dsp[i + 1];
			else
				next_val = curr_val;

			// Insert mid point
			ui_sw.fft_value[j + 1] = curr_val;

			// Get average for left point
			if(prev_val > curr_val)
			{
				aver = prev_val - curr_val;
				if(aver)
					aver = curr_val + aver/2;
				else
					aver = curr_val;
			}
			else
			{
				aver = curr_val - prev_val;
				if(aver)
					aver = prev_val + aver/2;
				else
					aver = curr_val;
			}

			// Insert left point
			ui_sw.fft_value[j + 0] = aver;

			// Get average for left point
			if(next_val > curr_val)
			{
				aver = next_val - curr_val;
				if(aver)
					aver = curr_val + aver/2;
				else
					aver = curr_val;
			}
			else
			{
				aver = curr_val - next_val;
				if(aver)
					aver = next_val + aver/2;
				else
					aver = curr_val;
			}

			// Insert right point
			ui_sw.fft_value[j + 2] = aver;

			// Next group
			j += 3;
		}

		// Emulate data in the right corner, outside of fft values
		for(i = 0; i < 14; i++)
		{
			if(i%5)// ToDo: fix this - call to C lib!!
				ui_sw.fft_value[j + i] = ui_sw.fft_dsp[255];
			else
				ui_sw.fft_value[j + i] = ui_sw.fft_dsp[255] + 1;
		}
	}
#endif
}

static void ui_controls_spectrum_fft_process_mid(void)
{
	ulong i,j;
	uchar curr_val,prev_val,next_val,aver;

	if(api_conv_type == 0)
	{
		// Copy to fft buffer
		// but cut left and right 16 points then expand
		for(i = 0, j = 0; i < 225; i++)
		{
			ui_sw.fft_value[j + 0] = 0 + ui_sw.fft_dsp[i + 16];
			ui_sw.fft_value[j + 1] = 0 + ui_sw.fft_dsp[i + 16];
			ui_sw.fft_value[j + 2] = 0 + ui_sw.fft_dsp[i + 16];
			j += 3;
		}
	}
	else
	{
		// Copy to fft buffer
		// 672 width of spectrum, expand 224 fft to 672 points
		// use approximation for adjacent points (only mid one is real value)
		for(i = 0, j = 0; i < 225; i++)
		{
			// Get current point
			curr_val = ui_sw.fft_dsp[i + 16];

			// Get previous point
			if(i == 0)
				prev_val = curr_val;
			else
				prev_val = ui_sw.fft_dsp[i - 1 + 16];

			// Get next point
			if(i < 254)
				next_val = ui_sw.fft_dsp[i + 1 + 16];
			else
				next_val = curr_val;

			// Insert mid point
			ui_sw.fft_value[j + 1] = curr_val;

			// Get average for left point
			if(prev_val > curr_val)
			{
				aver = prev_val - curr_val;
				if(aver)
					aver = curr_val + aver/2;
				else
					aver = curr_val;
			}
			else
			{
				aver = curr_val - prev_val;
				if(aver)
					aver = prev_val + aver/2;
				else
					aver = curr_val;
			}

			// Insert left point
			ui_sw.fft_value[j + 0] = aver;

			// Get average for left point
			if(next_val > curr_val)
			{
				aver = next_val - curr_val;
				if(aver)
					aver = curr_val + aver/2;
				else
					aver = curr_val;
			}
			else
			{
				aver = curr_val - next_val;
				if(aver)
					aver = next_val + aver/2;
				else
					aver = curr_val;
			}

			// Insert right point
			ui_sw.fft_value[j + 2] = aver;

			// Next group
			j += 3;
		}
	}
}

static void ui_controls_spectrum_decide_bandpass(void)
{
#if 1
	ushort bandpass_center 		= SPECTRUM_MID_POINT;
	uchar  bandpass_halfwidth  	= 20;

	// Calculate strip width
	switch(tsu.dsp_filter)
	{
		case AUDIO_300HZ:
			bandpass_halfwidth  = 3;
			break;
		case AUDIO_500HZ:
			bandpass_halfwidth  = 5;
			break;
		case AUDIO_1P8KHZ:
			bandpass_halfwidth  = 15;
			break;
		case AUDIO_2P3KHZ:
			bandpass_halfwidth  = 20;
			break;
		case AUDIO_3P6KHZ:
			bandpass_halfwidth  = 30;
			break;
		case AUDIO_WIDE:
			bandpass_halfwidth  = 80;
			break;
		default:
			break;
	}

	// If NCO is on, will move the bandstrip around
	if(tsu.band[tsu.curr_band].nco_freq != 0)
		bandpass_center = SPECTRUM_MID_POINT + ((tsu.band[tsu.curr_band].nco_freq/1000)*16);

	if(tsu.band[tsu.curr_band].demod_mode == DEMOD_LSB)
		bandpass_center -= bandpass_halfwidth;

	if(tsu.band[tsu.curr_band].demod_mode == DEMOD_USB)
		bandpass_center += bandpass_halfwidth;

	// Calculate boundary
	ui_sw.bandpass_start = (bandpass_center - bandpass_halfwidth);
	ui_sw.bandpass_end   = (bandpass_center + bandpass_halfwidth);
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_spectrum_repaint
//* Object              : - at 200MHz, 99% CPU usage and still jerky! -
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void ui_controls_spectrum_repaint_big(FAST_REFRESH *cb)
{
	GUI_RECT 	Rect;
	ulong 		i,j;
	int 		old_x,old_y,new_x,new_y;
	uchar		val;
	ushort		dif;

	//printf("paint spectrum\r\n");

	// --------------------------------------------------------------------------------------
	// Are we moving bandpass strip ?
	// Just a basic digitizer test
	// Check boundary - is touch event into this control at all ?
	/*if((t_d.point_y[0] < (SCOPE_Y + SCOPE_Y_SIZE)) && (t_d.point_y[0] > (SCOPE_Y + 0)))
	{
		// --------------------------------------------
		// Move bandpass - one finger
		if(t_d.count == 1)
		{
			bandpass_start = t_d.point_x[0];
			bandpass_end   = bandpass_start + 40;
		}

		// --------------------------------------------
		// Resizing bandpass via pinch/stretch ?
		if(t_d.count == 2)
		{
			if(t_d.point_x[0] > t_d.point_x[1])
				dif = t_d.point_x[0] - t_d.point_x[1];
			else
				dif = t_d.point_x[1] - t_d.point_x[0];

			// Expanding or contracting ?
			if(old_dif < dif)
			{
				bandpass_start -= 10;
				bandpass_end   += 10;
			}
			else
			{
				bandpass_start += 10;
				bandpass_end   -= 10;
			}
			old_dif = dif;
		}
	}*/
	// --------------------------------------------------------------------------------------

	// Select device
	GUI_MEMDEV_Select(hMemSpWf);

	// Draw background
	GUI_SetColor(GUI_BLACK);
	GUI_FillRect(	(SCOPE_X				),
					(SCOPE_Y				),
					(SCOPE_X + SCOPE_X_SIZE ),
					(SCOPE_Y + SCOPE_Y_SIZE )
				);

#if 1
	// Draw horizontal grid lines
	GUI_SetColor(GUI_DARKGRAY);
	for (i = 0; i < 7; i++)
		GUI_DrawHLine(((SCOPE_Y + SCOPE_Y_SIZE - 2) - (i * 16)),(SW_FRAME_X + SW_FRAME_WIDTH),(SW_FRAME_X + SW_FRAME_X_SIZE - 2));

#endif

#if 1
	// Draw vertical grid lines
	GUI_SetColor(GUI_DARKGRAY);
	for (i = 0; i < 9; i++)
	    GUI_DrawVLine((17 + i*96),SCOPE_Y,(SCOPE_Y + SCOPE_Y_SIZE - 2));

#endif

	// Draw bars
	for (i = 0; i < SCOPE_X_SIZE; i++)
	{
		val  = ui_sw.fft_value[i];

		// Top clipping
		if(val > SCOPE_Y_SIZE)
			val = SCOPE_Y_SIZE;

		// New point position
		new_x = (SCOPE_X + i);
		new_y = (SCOPE_Y + SCOPE_Y_SIZE - 2) - val;

#if 1
		// Print vertical line for each point, transparent, to fill the spectrum
		GUI_SetColor(GUI_LIGHTBLUE);
		//GUI_SetColor(GUI_WHITE);
		GUI_SetAlpha(88);	//168);
		GUI_DrawVLine(new_x,new_y,SCOPE_Y + SCOPE_Y_SIZE);
		GUI_SetAlpha(255);
#endif

#if 0
		// Draw point
		GUI_SetColor(GUI_WHITE);
		GUI_DrawPixel(new_x,new_y);
#endif

#if 1
		// Draw line between old and new point
		GUI_SetColor(GUI_WHITE);
		//GUI_SetColor(GUI_LIGHTBLUE);
		if(i) GUI_DrawLine(old_x,old_y,new_x,new_y);
#endif

		// Save old point
		old_x = new_x;
		old_y = new_y;

		// Fast UI update callback
		if(cb)
		{
			GUI_SelectLayer(0);
			cb();
			GUI_MEMDEV_Select(hMemSpWf);
		}
	}

	// Calculate bandpass strip boundaries
	ui_controls_spectrum_decide_bandpass();

#if 0
	if(!tsu.band[tsu.curr_band].fixed_mode)
	{
		// Draw alpha effect for bandpass
		Rect.x0 = SCOPE_X;
		Rect.x1 = SCOPE_X + 3;
		Rect.y0 = SCOPE_Y;
		Rect.y1 = SCOPE_Y + SCOPE_Y_SIZE;
		GUI_SetClipRect(&Rect);
		GUI_SetColor(0xd99100);
		GUI_SetAlpha(168);
		GUI_FillRoundedRect(SCOPE_X + 4, SCOPE_Y, SCOPE_X + SCOPE_X_SIZE, SCOPE_Y + SCOPE_Y_SIZE, 4);
		GUI_SetClipRect(NULL);
		// passband strip
		GUI_FillRect(SCOPE_X + ui_sw.bandpass_start, SCOPE_Y + 1, SCOPE_X + ui_sw.bandpass_end,   SCOPE_Y + SCOPE_Y_SIZE);
		// Red line - middle of passband
		GUI_SetColor(GUI_RED);
		GUI_DrawVLine((SCOPE_X + (ui_sw.bandpass_end - ui_sw.bandpass_start)/2 + ui_sw.bandpass_start),SCOPE_Y,SCOPE_Y + SCOPE_Y_SIZE);
		GUI_SetAlpha(0);
	}
	else
	{
		// Red needle, double width
		GUI_SetColor(GUI_RED);
		GUI_DrawVLine((SCOPE_X + (ui_sw.bandpass_end - ui_sw.bandpass_start)/2 + ui_sw.bandpass_start),SCOPE_Y,SCOPE_Y + SCOPE_Y_SIZE);
		GUI_DrawVLine((SCOPE_X + (ui_sw.bandpass_end - ui_sw.bandpass_start)/2 + ui_sw.bandpass_start + 1),SCOPE_Y,SCOPE_Y + SCOPE_Y_SIZE);
	}
#endif

#if 0
	// Transparent text - like band info,etc
	// looks extremly ugly in this configuration and spoils the spectrum control aesthetic!
	GUI_SetAlpha(140);
	GUI_SetFont(&GUI_Font24B_ASCII);
	GUI_SetColor(GUI_YELLOW);
	GUI_DispStringAt("this is a test for extra info",(SCOPE_X + 20),(SCOPE_Y + 42));
	GUI_SetAlpha(0);
#endif

	// Execute
	GUI_MEMDEV_CopyToLCD(hMemSpWf);

	// Allow normal text print in other controls
	GUI_SelectLayer(0);

#if 0
	int x0,y0,FontSizeY;

	// Bottom divider below waterfall
	//GUI_SetColor(GUI_DARKGRAY);
	//GUI_DrawHLine((WATERFALL_Y + WATERFALL_Y_SIZE), (WATERFALL_X + SW_FRAME_WIDTH),(WATERFALL_X + WATERFALL_X_SIZE - SW_FRAME_WIDTH));
	//GUI_FillRoundedRect(		(WATERFALL_X + 0),
	//							(WATERFALL_Y + WATERFALL_Y_SIZE),
	//							(WATERFALL_X + WATERFALL_X_SIZE - 1),
	//							(WATERFALL_Y + WATERFALL_Y_SIZE) + 17,2);

	// Bottom Labels
	GUI_SetAlpha(168);
	GUI_SetFont(&GUI_Font8x16_1);
	GUI_SetColor(GUI_ORANGE);
	FontSizeY = GUI_GetFontSizeY();
	for (i = 0, j = -24; i < 9; i++)
	{
	    x0 = 10 + i*96;
	    y0 = (WATERFALL_Y + WATERFALL_Y_SIZE) + 4;
	    GUI_GotoXY(x0 + 8, y0 + 7 - FontSizeY / 2);
	    GUI_SetTextAlign(GUI_TA_HCENTER);
	    GUI_DispDecMin(j + 6*i);
	    GUI_DrawVLine((x0 + 7),(y0 - 4),(y0 - 1));
	}
	GUI_SetAlpha(0);
#endif
}

static void ui_controls_spectrum_repaint_mid(FAST_REFRESH *cb)
{
	GUI_RECT 	Rect;
	ulong 		i,j;
	int 		old_x,old_y,new_x,new_y;
	uchar		val;
	ushort		dif;

	// Select device
	GUI_MEMDEV_Select(hMemSpWf);

	// Draw background
	GUI_SetColor(GUI_BLACK);
	GUI_FillRect(	(SCOPE_X_MID				),
					(SCOPE_Y_MID				),
					(SCOPE_X_MID + SCOPE_X_SIZE_MID ),
					(SCOPE_Y_MID + SCOPE_Y_SIZE_MID )
				);

	// Is it light version of the control ?
	if(!sw_light)
	{
		// Draw horizontal grid lines
		GUI_SetColor(GUI_DARKGRAY);
		for (i = 0; i < 7; i++)
			GUI_DrawHLine(((SCOPE_Y_MID + SCOPE_Y_SIZE_MID - 2) - (i * 16)),(SW_FRAME_X_MID + SW_FRAME_WIDTH_MID),(SW_FRAME_X_MID + SW_FRAME_X_SIZE_MID - 2));

		// Draw vertical grid lines
		GUI_SetColor(GUI_DARKGRAY);
		for (i = 0; i < 9; i++)
			GUI_DrawVLine((19 + i*80),SCOPE_Y_MID,(SCOPE_Y_MID + SCOPE_Y_SIZE_MID - 2));
	}

	// Draw bars
	for (i = 0,j = 0; i < SCOPE_X_SIZE_MID; i++,j++)
	{
		// Freq/4
		if(j == 4) j = 0;

		val  = ui_sw.fft_value[i];

		if(!sw_light)
		{
			// Top clipping
			if(val > SCOPE_Y_SIZE_MID)
				val = SCOPE_Y_SIZE_MID;
		}
		else
		{
			// Top clipping
			if(val > SCOPE_Y_SIZE_MID/2)
				val = SCOPE_Y_SIZE_MID/2;
		}

		// New point position
		new_x = (SCOPE_X_MID + i);
		new_y = (SCOPE_Y_MID + SCOPE_Y_SIZE_MID - 2) - val;

		if(!sw_light)
		{
			// Print vertical line for each point, transparent, to fill the spectrum
			GUI_SetColor(GUI_LIGHTBLUE);
			//GUI_SetColor(GUI_WHITE);
			GUI_SetAlpha(88);	//168);
			GUI_DrawVLine(new_x,new_y,SCOPE_Y_MID + SCOPE_Y_SIZE_MID);
			GUI_SetAlpha(255);
		}

		// If in low resource mode & correct frequency
		if((sw_light) && (!j))
		{
			// Draw point
			GUI_SetColor(GUI_WHITE);
			GUI_DrawPixel(new_x,new_y);
		}

		if(!sw_light)
		{
			// Draw line between old and new point
			GUI_SetColor(GUI_WHITE);
			//GUI_SetColor(GUI_LIGHTBLUE);
			if(i) GUI_DrawLine(old_x,old_y,new_x,new_y);
		}

		// Save old point
		old_x = new_x;
		old_y = new_y;

		// Fast UI update callback
		if(cb)
		{
			GUI_SelectLayer(0);
			cb();
			GUI_MEMDEV_Select(hMemSpWf);
		}
	}

	if(!sw_light)
	{
		// Draw alpha effect for bandpass
		Rect.x0 = SCOPE_X;
		Rect.x1 = SCOPE_X + 3;
		Rect.y0 = SCOPE_Y;
		Rect.y1 = SCOPE_Y + SCOPE_Y_SIZE;
		GUI_SetClipRect(&Rect);
		GUI_SetColor(0xd99100);
		GUI_SetAlpha(88);	//168);
		GUI_FillRoundedRect(SCOPE_X_MID + 4, SCOPE_Y_MID, SCOPE_X_MID + SCOPE_X_SIZE_MID, SCOPE_Y_MID + SCOPE_Y_SIZE_MID, 4);
		GUI_SetClipRect(NULL);
		// passband strip
		GUI_FillRect(SCOPE_X_MID + ui_sw.bandpass_start, SCOPE_Y_MID + 1, SCOPE_X_MID + ui_sw.bandpass_end,   SCOPE_Y_MID + SCOPE_Y_SIZE_MID);
		// Red line - middle of passband
		GUI_SetColor(GUI_RED);
		GUI_DrawVLine((SCOPE_X_MID + (ui_sw.bandpass_end - ui_sw.bandpass_start)/2 + ui_sw.bandpass_start),SCOPE_Y,SCOPE_Y_MID + SCOPE_Y_SIZE_MID);
		GUI_SetAlpha(255);
	}

	if(sw_light)
	{
		// Transparent text - like band info,etc
		// looks extremely ugly in this configuration and spoils the spectrum control aesthetic!
		GUI_SetAlpha(88);	//168);
		GUI_SetFont(&GUI_Font24B_ASCII);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt("SW control in low resource mode, maybe CW decode here ?",(SCOPE_X_MID + 20),(SCOPE_Y_MID + 20));
		GUI_SetAlpha(255);
	}

	// Execute
	GUI_MEMDEV_CopyToLCD(hMemSpWf);

	// Allow normal text print in other controls
	GUI_SelectLayer(0);
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_spectrum_wf_repaint
//* Object              : - at 200MHz, 86% CPU usage, speed is good!
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void ui_controls_spectrum_wf_repaint_big(void)
{
	ulong 		i,j,m;
	ulong		val;
	ulong 		y_copy_sz = 1;	// only one line at a time seems to only work (driver LCD_Copy problem maybe)

	// Move down
	for (i = WATERFALL_Y_SIZE; i > 0; i -= y_copy_sz)
		GUI_CopyRect(WATERFALL_X,WATERFALL_Y-y_copy_sz+i,WATERFALL_X,WATERFALL_Y+i,WATERFALL_X_SIZE,y_copy_sz);

	// Update current line
	for (i = 0; i < WATERFALL_X_SIZE; i++)
	{
		val  = ui_sw.fft_value[i];
		val &= 0x3F;

		// ToDo: Fix gradient table addressing
		GUI_SetColor ((0xFF << 24) | waterfall_blue[val]);	// add Alpha value
		GUI_DrawPixel((WATERFALL_X + i), WATERFALL_Y);
	}
}

static void ui_controls_spectrum_wf_repaint_mid(void)
{
	ulong 		i,j,m;
	ulong		val;

	// -----------------------------------------------------------------------------------------------------------
	// Move waterfall down
	// Our screen is swapped (LCD_LL_CopyRect() in lcd.c reflects that), that is why weird x and y positions!
	//
	GUI_CopyRect(	(800 - WATERFALL_X_MID - WATERFALL_X_SIZE_MID),		// Upper left X-position of the source rectangle.
					(WATERFALL_Y_MID + WATERFALL_Y_SIZE_MID +   0),		// Upper left Y-position of the source rectangle.
					(800 - WATERFALL_X_MID - WATERFALL_X_SIZE_MID),		// Upper left X-position of the destination rectangle.
					(WATERFALL_Y_MID + WATERFALL_Y_SIZE_MID +   1),		// Upper left Y-position of the destination rectangle.
					(WATERFALL_X_SIZE_MID					+   4),		// X-size of the rectangle.
					(WATERFALL_Y_SIZE_MID					+   0)		// Y-size of the rectangle.
				);

	// Print current waterfall line (top row)
	// but only after moving the old waterfall down
	for (i = 0; i < WATERFALL_X_SIZE_MID; i++)
	{
		val  = ui_sw.fft_value[i];
		val &= 0x3F;

		// ToDo: Fix gradient table addressing
		GUI_SetColor(waterfall_blue[val]);

		GUI_DrawPixel((WATERFALL_X_MID + i),(WATERFALL_Y_MID));
	}
}

static void ui_controls_update_smooth_control(uchar init)
{
	uchar pub_smooth;

	pub_smooth = *(uchar *)(EEP_BASE + EEP_SW_SMOOTH);

	if(!init)
	{
		if(api_conv_type == pub_smooth)
			return;
	}
	//printf("smooth=%d\r\n",api_conv_type);

	// 'Smooth' indicator holder
	GUI_SetColor(GUI_LIGHTBLUE);

	if(ui_sw.ctrl_type == SW_CONTROL_BIG)
		GUI_FillRoundedRect(SMOOTH_X,SMOOTH_Y,(SMOOTH_X + SMOOTH_X_SIZE),(SMOOTH_Y + SMOOTH_Y_SIZE),2);
	else
		GUI_FillRoundedRect(SMOOTH_X_MID,SMOOTH_Y_MID,(SMOOTH_X_MID + SMOOTH_X_SIZE_MID),(SMOOTH_Y_MID + SMOOTH_Y_SIZE_MID),2);

	// Color based on public state
	if(pub_smooth)
		GUI_SetColor(GUI_WHITE);
	else
		GUI_SetColor(GUI_GRAY);

	// 'Smooth' indicator text
	GUI_SetFont(&GUI_Font16B_ASCII);

	if(ui_sw.ctrl_type == SW_CONTROL_BIG)
		GUI_DispStringAt("SMOOTH",(SMOOTH_X + 5),(SMOOTH_Y + 0));
	else
		GUI_DispStringAt("SMOOTH",(SMOOTH_X_MID + 5),(SMOOTH_Y_MID + 0));

	//loc_smooth_val = api_conv_type;

	// Flip flag
	//api_conv_type = !api_conv_type;

	api_conv_type = pub_smooth;
}

// Center/Fixed
//
static void ui_controls_update_vfo_mode(bool is_init)
{
#if 1
	// Skip needless repaint
	if((!is_init) && (loc_vfo_mode == tsu.band[tsu.curr_band].fixed_mode))
		return;

	//printf("update vfo mode\r\n");

	// 'Smooth' indicator holder
	GUI_SetColor(GUI_WHITE);

	if(ui_sw.ctrl_type == SW_CONTROL_BIG)
		GUI_FillRect(CENTER_X,CENTER_Y,(CENTER_X + CENTER_X_SIZE),(CENTER_Y + CENTER_Y_SIZE));
	else
		GUI_FillRoundedRect(CENTER_X_MID,CENTER_Y_MID,(CENTER_X_MID + CENTER_X_SIZE_MID),(CENTER_Y_MID + CENTER_Y_SIZE_MID),2);

	GUI_SetColor(GUI_LIGHTBLUE);
	GUI_SetFont(&GUI_Font16B_ASCII);

	if(ui_sw.ctrl_type == SW_CONTROL_BIG)
	{
		if(tsu.band[tsu.curr_band].fixed_mode)
			GUI_DispStringAt("FIXED",(CENTER_X + 14),(CENTER_Y + 0));
		else
			GUI_DispStringAt("CENTER",(CENTER_X + 8),(CENTER_Y + 0));
	}
	else
	{
		if(tsu.band[tsu.curr_band].fixed_mode)
			GUI_DispStringAt("FIXED",(CENTER_X_MID + 14),(CENTER_Y_MID + 0));
		else
			GUI_DispStringAt("CENTER",(CENTER_X_MID + 8),(CENTER_Y_MID + 0));
	}

	loc_vfo_mode = tsu.band[tsu.curr_band].fixed_mode;
#endif
}

static void ui_controls_create_header_big(void)
{
	// Left part
	GUI_DrawGradientRoundedH(	(SW_FRAME_X + SW_FRAME_WIDTH),
								(SW_FRAME_Y + 2),
								(SW_FRAME_X + SW_FRAME_X_SIZE - 2)/2 + 10,
								(SW_FRAME_Y + 22),
								2,
								GUI_DARKGRAY,
								GUI_GRAY
						);

	// Right part
	GUI_DrawGradientRoundedH(	(SW_FRAME_X + SW_FRAME_X_SIZE - 2)/2,
								(SW_FRAME_Y + 2),
								(SW_FRAME_X + SW_FRAME_X_SIZE - 2),
								(SW_FRAME_Y + 22),
								2,
								GUI_GRAY,
								GUI_DARKGRAY
							);

	// Centre text
	GUI_SetFont(&GUI_Font20B_ASCII);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt("SPECTRUM SCOPE",((SW_FRAME_X + SW_FRAME_X_SIZE - 2)/2 - 80),(SW_FRAME_Y + 2));

	// Display 'Span' static control
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_Font16B_ASCII);
	GUI_DispStringAt("SPAN 48kHz",(SPAN_X + 12),(SPAN_Y + 0));

	// Create smooth control
	ui_controls_update_smooth_control(1);
	ui_controls_update_vfo_mode(true);
}

static void ui_controls_create_header_mid(void)
{
	// Left part
	GUI_DrawGradientRoundedH(	(SW_FRAME_X_MID + SW_FRAME_WIDTH_MID),
								(SW_FRAME_Y_MID + 2),
								(SW_FRAME_X_MID + SW_FRAME_X_SIZE_MID - 2)/2 + 10,
								(SW_FRAME_Y_MID + 22),
								2,
								GUI_DARKGRAY,
								GUI_GRAY
						);

	// Right part
	GUI_DrawGradientRoundedH(	(SW_FRAME_X_MID + SW_FRAME_X_SIZE_MID - 2)/2,
								(SW_FRAME_Y_MID + 2),
								(SW_FRAME_X_MID + SW_FRAME_X_SIZE_MID - 2),
								(SW_FRAME_Y_MID + 22),
								2,
								GUI_GRAY,
								GUI_DARKGRAY
							);

	// Centre text
	GUI_SetFont(&GUI_Font20B_ASCII);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt("SCOPE",((SW_FRAME_X_MID + SW_FRAME_X_SIZE_MID - 2)/2 - 0),(SW_FRAME_Y_MID + 2));

	// Display 'Span' static control
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_Font16B_ASCII);
	GUI_DispStringAt("SPAN 40kHz",(SPAN_X_MID + 12),(SPAN_Y_MID + 0));

	// Create smooth control
	ui_controls_update_smooth_control(1);
	ui_controls_update_vfo_mode(true);
}

static void ui_controls_create_sw_big(void)
{
	int i,j,x0,y0,FontSizeY;

	// Create memory device (for spectrum only!)
	if (hMemSpWf == 0)
	{
		hMemSpWf = GUI_MEMDEV_Create(	SCOPE_X,
										SCOPE_Y,
										SCOPE_X_SIZE,
										SCOPE_Y_SIZE
									);
		if (hMemSpWf == 0)
		{
			printf("err mem dev!\r\n");

			// ToDo: stall or handle error!
			return; // Not enough memory available
		}
	}

	// Select device
	GUI_MEMDEV_Select(hMemSpWf);

	// Initial clear of control
	GUI_SetColor(GUI_BLACK);
	GUI_FillRect(	SCOPE_X,
					SCOPE_Y,
					(SCOPE_X + SCOPE_X_SIZE),
					(SCOPE_Y + SCOPE_Y_SIZE)
				);

	// Execute
	GUI_MEMDEV_CopyToLCD(hMemSpWf);

    // Initial control paint
	ui_controls_spectrum_repaint_big(NULL);
	ui_controls_spectrum_wf_repaint_big();

    // Allow normal text print in other controls
    GUI_SelectLayer(0);

	// Draw orange frame
	GUI_SetColor(GUI_ORANGE);
	GUI_DrawRoundedFrame(	SW_FRAME_X,
							SW_FRAME_Y,
							(SW_FRAME_X + SW_FRAME_X_SIZE),
							(SW_FRAME_Y + SW_FRAME_Y_SIZE),
							SW_FRAME_CORNER_R,
							SW_FRAME_WIDTH
						);

	// Draw header
	ui_controls_create_header_big();

#if 1
	// Bottom divider below waterfall
	GUI_SetColor(GUI_DARKGRAY);
	//GUI_DrawHLine((WATERFALL_Y + WATERFALL_Y_SIZE), (WATERFALL_X + SW_FRAME_WIDTH),(WATERFALL_X + WATERFALL_X_SIZE - SW_FRAME_WIDTH));
	GUI_FillRoundedRect(		(WATERFALL_X + 0),
								(WATERFALL_Y + WATERFALL_Y_SIZE),
								(WATERFALL_X + WATERFALL_X_SIZE - 1),
								(WATERFALL_Y + WATERFALL_Y_SIZE) + 17,2);

	// Bottom Labels
	GUI_SetFont(&GUI_Font8x16_1);
	GUI_SetColor(GUI_ORANGE);
	FontSizeY = GUI_GetFontSizeY();
	for (i = 0, j = -24; i < 9; i++)
	{
	    x0 = 10 + i*96;
	    y0 = (WATERFALL_Y + WATERFALL_Y_SIZE) + 4;
	    GUI_GotoXY(x0 + 8, y0 + 7 - FontSizeY / 2);
	    GUI_SetTextAlign(GUI_TA_HCENTER);
	    GUI_DispDecMin(j + 6*i);
	    GUI_DrawVLine((x0 + 7),(y0 - 4),(y0 - 1));
	}
#endif
}

static void ui_controls_create_sw_mid(void)
{
	int i,j,x0,y0,FontSizeY;

	// Create memory device (for spectrum only!)
	if (hMemSpWf == 0)
	{
		hMemSpWf = GUI_MEMDEV_Create(	SCOPE_X_MID,
										SCOPE_Y_MID,
										SCOPE_X_SIZE_MID,
										SCOPE_Y_SIZE_MID
									);
		if (hMemSpWf == 0)
		{
			// ToDo: stall or handle error!
			return; // Not enough memory available
		}
	}

	// Select device
	GUI_MEMDEV_Select(hMemSpWf);

	// Initial clear of control
	GUI_SetColor(GUI_BLACK);
	GUI_FillRect(	SCOPE_X_MID,
					SCOPE_Y_MID,
					(SCOPE_X_MID + SCOPE_X_SIZE_MID),
					(SCOPE_Y_MID + SCOPE_Y_SIZE_MID)
				);

	// Execute
	GUI_MEMDEV_CopyToLCD(hMemSpWf);

    // Initial control paint
	ui_controls_spectrum_repaint_mid(NULL);
	//ui_controls_spectrum_wf_repaint_big();

    // Allow normal text print in other controls
    GUI_SelectLayer(0);

	// Draw orange frame
	GUI_SetColor(GUI_ORANGE);
	GUI_DrawRoundedFrame(	SW_FRAME_X_MID,
							SW_FRAME_Y_MID,
							(SW_FRAME_X_MID + SW_FRAME_X_SIZE_MID),
							(SW_FRAME_Y_MID + SW_FRAME_Y_SIZE_MID),
							SW_FRAME_CORNER_R_MID,
							SW_FRAME_WIDTH_MID
						);

	// Draw header
	ui_controls_create_header_mid();


	// Bottom divider below waterfall
	GUI_SetColor(GUI_DARKGRAY);
	//GUI_DrawHLine((WATERFALL_Y + WATERFALL_Y_SIZE), (WATERFALL_X + SW_FRAME_WIDTH),(WATERFALL_X + WATERFALL_X_SIZE - SW_FRAME_WIDTH));
	GUI_FillRoundedRect(		(WATERFALL_X_MID + 0),
								(WATERFALL_Y_MID + WATERFALL_Y_SIZE_MID),
								(WATERFALL_X_MID + WATERFALL_X_SIZE_MID - 1),
								(WATERFALL_Y_MID + WATERFALL_Y_SIZE_MID) + 17,2);
	// Bottom Labels
	GUI_SetFont(&GUI_Font8x16_1);
	GUI_SetColor(GUI_ORANGE);
	FontSizeY = GUI_GetFontSizeY();
	for (i = 0, j = -20; i < 9; i++)
	{
	    x0 = 10 + i*80;
	    y0 = (WATERFALL_Y_MID + WATERFALL_Y_SIZE_MID) + 4;
	    GUI_GotoXY(x0 + 9, y0 + 7 - FontSizeY / 2);
	    GUI_SetTextAlign(GUI_TA_HCENTER);
	    GUI_DispDecMin(j + 5*i);
	    GUI_DrawVLine((x0 + 8),(y0 - 4),(y0 - 1));
	}
}

void ui_controls_spectrum_init(void)
{
	loc_vfo_mode = 0x99;

	// Control size based on saved eeprom value - only in CW mode ??
	//if(*(uchar *)(EEP_BASE + EEP_KEYER_ON))
	//	ui_sw.ctrl_type = SW_CONTROL_MID;
	//else
		ui_sw.ctrl_type = SW_CONTROL_BIG;

	ui_sw.bandpass_start 	= SPECTRUM_MID_POINT - SPECTRUM_DEF_HALF_BW*2;
	ui_sw.bandpass_end 		= SPECTRUM_MID_POINT;

	switch(ui_sw.ctrl_type)
	{
		case SW_CONTROL_BIG:
			ui_controls_create_sw_big();
			break;
		case SW_CONTROL_MID:
			ui_controls_create_sw_mid();
			break;
		case SW_CONTROL_SMALL:
			//ui_controls_create_sw_big();
			break;
		default:
			break;
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_spectrum_quit
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void ui_controls_spectrum_quit(void)
{
	GUI_MEMDEV_Delete(hMemSpWf);
	hMemSpWf = 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_spectrum_touch
//* Object              :
//* Notes    			: since we have more than one child control
//* Notes   			: we have yet another touch router
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_spectrum_touch(void)
{
	//printf("spectrum touched\r\n");
#if 0
	if(ui_sw.ctrl_type == SW_CONTROL_BIG)
	{
		// ---------------
		// Smooth button control
		if(		(t_d.point_x[0] >= SMOOTH_X) &&\
				(t_d.point_x[0] <= (SMOOTH_X + SMOOTH_X_SIZE)) &&\
				(t_d.point_y[0] >= SMOOTH_Y) &&\
				(t_d.point_y[0] <= (SMOOTH_Y + SMOOTH_Y_SIZE))
		  )
		{
			//printf("smooth button touched\r\n");
			ui_controls_update_smooth_control();
		}
	}
	if(ui_sw.ctrl_type == SW_CONTROL_MID)
	{
		// ---------------
		// Smooth button control
		if(		(t_d.point_x[0] >= SMOOTH_X_MID) &&\
				(t_d.point_x[0] <= (SMOOTH_X_MID + SMOOTH_X_SIZE_MID)) &&\
				(t_d.point_y[0] >= SMOOTH_Y_MID) &&\
				(t_d.point_y[0] <= (SMOOTH_Y_MID + SMOOTH_Y_SIZE_MID))
		  )
		{
			//printf("smooth button touched\r\n");
			ui_controls_update_smooth_control();
		}
	}
#endif
#if 0
	if(ui_sw.ctrl_type == SW_CONTROL_BIG)
	{
		// ---------------
		// Center button control
		if(		(t_d.point_x[0] >= CENTER_X) &&\
				(t_d.point_x[0] <= (CENTER_X + CENTER_X_SIZE)) &&\
				(t_d.point_y[0] >= CENTER_Y) &&\
				(t_d.point_y[0] <= (CENTER_Y + CENTER_Y_SIZE))
		  )
		{
			//printf("smooth button touched\r\n");
			ui_controls_update_vfo_mode(false);
		}
	}
	// ToDo: implement MID version as well...
#endif

#if 0
	// - ToDo: only in center mode!
	//         als0 check screen coordinates !!!
	// Test, move NCO
	//bandpass_center = SPECTRUM_MID_POINT + ((tsu.nco_freq/1000)*16);
	if(t_d.point_x[0] < 400)
	{
		tsu.band[tsu.curr_band].nco_freq = -(((400 - t_d.point_x[0])*1000)/16);
	}
	else
	{
		if(t_d.point_x[0] == 400)
			tsu.band[tsu.curr_band].nco_freq = 0;
		else
			tsu.band[tsu.curr_band].nco_freq = ((t_d.point_x[0] - 400)*1000)/16;
	}

#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_spectrum_refresh
//* Object              :
//* Notes    			: fast repaint ends up here
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_spectrum_refresh(FAST_REFRESH *cb)
{
	ui_controls_update_vfo_mode(false);
	ui_controls_update_smooth_control(0);

	switch(ui_sw.ctrl_type)
	{
		case SW_CONTROL_BIG:
			ui_controls_spectrum_fft_process_big();
			ui_controls_spectrum_repaint_big(cb);
			ui_controls_spectrum_wf_repaint_big();
			break;
#if 0
		case SW_CONTROL_MID:
			ui_controls_spectrum_fft_process_mid();
			ui_controls_spectrum_repaint_mid(cb);
			ui_controls_spectrum_wf_repaint_mid();
			break;
		case SW_CONTROL_SMALL:
			//ui_controls_create_sw_big();
			break;
#endif
		default:
			break;
	}
}

#endif
