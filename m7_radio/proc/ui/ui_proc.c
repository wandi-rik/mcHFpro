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

#include "mchf_pro_board.h"
#include "main.h"

#ifdef CONTEXT_DRIVER_UI

#include "ui_proc.h"
//
//#include "touch_driver.h"
//#include "hw\rtc\k_rtc.h"
//#include "hw\dsp_eep\hw_dsp_eep.h"
//
#include "gui.h"
#include "dialog.h"
//#include "ST_GUI_Addons.h"

// -----------------------------------------------------------------------------------------------
// Desktop Mode
#include "ui_controls_layout.h"
#include "spectrum\ui_controls_spectrum.h"
#include "smeter\ui_controls_smeter.h"
#include "freq\ui_controls_frequency.h"
#include "volume\ui_controls_volume.h"
#include "clock\ui_controls_clock.h"
#include "band\ui_controls_band.h"
#include "keyer\ui_controls_keyer.h"
#include "filter\ui_controls_filter.h"
#include "vfo_step\ui_controls_vfo_step.h"
#include "rx_tx\ui_controls_rx_tx.h"
#include "demod\ui_controls_demod.h"
#include "cpu_stat\ui_controls_cpu_stat.h"
#include "dsp_stat\ui_controls_dsp_stat.h"
#include "sd_icon\ui_controls_sd_icon.h"
#include "agc\ui_controls_agc.h"
#include "wifi_stat\ui_controls_wifi.h"
// -----------------------------------------------------------------------------------------------
// Side Encoder Options Menu
//#include "side_enc_menu\ui_side_enc_menu.h"
// -----------------------------------------------------------------------------------------------
// Quick Log Entry Menu
//#include "quick_log_entry\ui_quick_log.h"
// -----------------------------------------------------------------------------------------------
// FT8 Desktop
//#include "desktop_ft8\ui_desktop_ft8.h"
// -----------------------------------------------------------------------------------------------
// Menu Mode
#include "menu\k_module.h"
#include "menu\ui_menu.h"
// Menu items
extern K_ModuleItem_Typedef  	dsp_s;				// Standard DSP Menu
extern K_ModuleItem_Typedef  	dsp_e;				// Extended DSP Menu
extern K_ModuleItem_Typedef  	user_i;				// User Interface
//extern K_ModuleItem_Typedef  	file_b;				// File Browser
extern K_ModuleItem_Typedef  	clock;				// Clock Settings
extern K_ModuleItem_Typedef  	reset;				// Factory Reset
//extern K_ModuleItem_Typedef  	wsjt;				// WSJT-X Tools
//extern K_ModuleItem_Typedef  	logbook;			// Logbook
extern K_ModuleItem_Typedef  	info;				// System Information
// -----------------------------------------------------------------------------------------------
// Splash Screen
//#include "splash\ui_startup.h"
// -----------------------------------------------------------------------------------------------

// UI driver public state
struct	UI_DRIVER_STATE			ui_s;

// Touch data - emWin
GUI_PID_STATE 					TS_State;
// Touch driver state - our driver
//extern struct TD 				t_d;

// Public radio state
//extern struct	TRANSCEIVER_STATE_UI	tsu;

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
// All ui call that take too long should call here to release
// control to functions that need real time reaction
void ui_callback_one(void)
{
#if 0
	//--printf("callback1\r\n");
	#ifdef VFO_BOTH
	ui_controls_frequency_refresh(0);
	#endif
	ui_controls_clock_refresh();

	ui_controls_cpu_stat_refresh();

	//ui_driver_change_screen_demod_mode();
	//ui_controls_band_refresh();
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void ui_callback_two(void)
{
#if 0
	//--printf("callback2\r\n");
	#ifdef VFO_BOTH
	ui_controls_frequency_refresh(0);
	#endif
	ui_controls_clock_refresh();

	//ui_driver_change_screen_demod_mode();
	//ui_controls_band_refresh();
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : _cbBk
//* Object              : main windows callback, needed for v5.44 2D lib refresh
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void _cbBk(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
			break;

		case WM_PAINT:
		{
			// To prevent lag on frequency update, always call here
			#ifdef VFO_BOTH
			ui_controls_frequency_refresh(0);
			#endif

			//if(*(uchar *)(EEP_BASE + EEP_KEYER_ON))
			//	ui_controls_keyer_refresh();

			#ifdef SPECTRUM_WATERFALL
			ui_controls_spectrum_refresh(&ui_callback_two);
			#endif

			ui_controls_smeter_refresh  (&ui_callback_one);

			ui_controls_dsp_stat_refresh();
			ui_controls_cpu_stat_refresh();
			//--ui_controls_volume_refresh();
			ui_controls_demod_refresh();
			ui_controls_band_refresh();
			ui_controls_filter_refresh();
			ui_controls_vfo_step_refresh();
			ui_controls_clock_refresh();
			//ui_controls_sd_icon_refresh();
			ui_controls_agc_refresh();

			break;
		}

		default:
			break;
	}
}


//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void ui_driver_init_desktop(void)
{
	WM_SetCallback(WM_HBKWIN, _cbBk);

	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();

	ui_controls_clock_init();

	#ifdef SPECTRUM_WATERFALL
	ui_controls_spectrum_init();
	#endif

	ui_controls_smeter_init();

	#ifdef VFO_BOTH
	ui_controls_frequency_init();
	#endif

	ui_controls_wifi_init(WM_HBKWIN);
	ui_controls_volume_init();
	ui_controls_band_init();
	ui_controls_filter_init();
	ui_controls_vfo_step_init();
	ui_controls_rx_tx_init();
	ui_controls_demod_init();
	ui_controls_cpu_stat_init();
	ui_controls_dsp_stat_init();
	//ui_controls_sd_icon_init();
	ui_controls_agc_init();

//	if(*(uchar *)(EEP_BASE + EEP_KEYER_ON))
//		ui_controls_keyer_init();

	GUI_Exec();
}

#if 1
//*----------------------------------------------------------------------------
//* Function Name       : ui_driver_change_mode
//* Object              : change screen mode
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void ui_driver_change_mode(void)
{
	uchar state;

	// Take a snapshot of the state
	state = ui_s.req_state;

	// Do we need update ?
	if(ui_s.cur_state == state)
		return;

	switch(state)
	{
		// Switch to menu mode
		case MODE_MENU:
		{
			printf("Entering Menu mode...\r\n");

			// Destroy desktop controls
			ui_controls_volume_quit();
			ui_controls_wifi_quit();
			ui_controls_smeter_quit();
			ui_controls_spectrum_quit();

			WM_SetCallback		(WM_HBKWIN, 0);
			WM_InvalidateWindow	(WM_HBKWIN);

			// Clear screen
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();

			// Set General Graphical properties
			ui_set_gui_profile();

			// Show the main menu
			ui_init_menu();

			// Initial paint
			GUI_Exec();

			break;
		}

#if 0
		// Switch to side encoder options mode
		case MODE_SIDE_ENC_MENU:
		{
			printf("Entering Side encoder options mode...\r\n");

			// Destroy desktop controls
			ui_controls_smeter_quit();
			ui_controls_spectrum_quit();

			// Clear screen
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();

			// Show popup
			ui_side_enc_menu_create();

			// Initial paint
			GUI_Exec();

			break;
		}
#endif
#if 0
		// Switch to FT8 mode
		case MODE_DESKTOP_FT8:
		{
			printf("Entering FT8 mode...\r\n");

			// Destroy desktop controls
			ui_controls_smeter_quit();
			ui_controls_spectrum_quit();

			// Clear screen
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();

			// Show popup
			ui_desktop_ft8_create();

			// Initial paint
			GUI_Exec();

			break;
		}
#endif
#if 0
		case MODE_QUICK_LOG:
		{
			printf("Entering Quick Log Entry mode...\r\n");

			// Destroy desktop controls
			ui_controls_smeter_quit();
			ui_controls_spectrum_quit();

			// Clear screen
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();

			// Show popup
			ui_quick_log_create();

			// Initial paint
			GUI_Exec();

			break;
		}
#endif
		// Switch to desktop mode
		case MODE_DESKTOP:
		{
			printf("Entering Desktop mode...\r\n");

			// Destroy any Window Manager items
			ui_destroy_menu();
//!			ui_side_enc_menu_destroy();
//!			ui_desktop_ft8_destroy();
//!			ui_quick_log_destroy();

			// Clear screen
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();

			// Init controls
			ui_driver_init_desktop();

			break;
		}

		default:
			break;
	}

	// Update flag
	ui_s.cur_state = state;

	// Release lock
	ui_s.lock_requests = 0;
}
#endif

#if 0
//*----------------------------------------------------------------------------
//* Function Name       : ui_driver_touch_router
//* Object              : -- route touch events to user controls --
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
static void ui_driver_touch_router(void)
{
	// ---------------------------------------------------------
	// --------------		MENU MODE        -------------------
	// ---------------------------------------------------------
	//
	// Submit to emWin
	//
	// - the reason this is here, instead of the touch driver -
	//   emWin doesn't like being accessed from separate threads
	//   and we don't want critical sections on touch events!
	//
	if(		(ui_s.cur_state == MODE_MENU)||\
			(ui_s.cur_state == MODE_SIDE_ENC_MENU)||\
			(ui_s.cur_state == MODE_QUICK_LOG)||\
			(ui_s.cur_state == MODE_DESKTOP_FT8))
	{
		// Process pending from digitizer driver
		if((t_d.pending) && (!TS_State.Pressed))
		{
			//printf("pass touch event to emWin\r\n");

			TS_State.Pressed 	= 1;
			TS_State.Layer 		= 0;
			TS_State.x 			= t_d.point_x[0];
			TS_State.y 			= t_d.point_y[0];

			GUI_TOUCH_StoreStateEx(&TS_State);

			// Reset flag
			t_d.pending 		= 0;

			return;
		}

		// emWin needs depress event, so generate on next call
		if((!t_d.pending) && (TS_State.Pressed))
		{
			//printf("clear touch event in emWin\r\n");

			TS_State.Pressed 	= 0;
			TS_State.Layer 		= 0;
			TS_State.x 			= 0;
			TS_State.y 			= 0;

			GUI_TOUCH_StoreStateEx(&TS_State);

			return;
		}

		// Just in case
		return;
	}

	// ---------------------------------------------------------
	// --------------		DESKTOP MODE     -------------------
	// ---------------------------------------------------------
	//
	// Anything waiting ?
	if(t_d.pending == 0)
		return;
#if 1
	// ---------------------------------------------------------
	// Process on screen keyer
	if(		(t_d.point_x[0] >= IAMB_KEYER_X) &&\
			(t_d.point_x[0] <= (IAMB_KEYER_X + IAMB_KEYER_SIZE_X)) &&\
			(t_d.point_y[0] >= IAMB_KEYER_Y) &&\
			(t_d.point_y[0] <= (IAMB_KEYER_Y + IAMB_KEYER_SIZE_Y))
	  )
	{
//		ui_controls_keyer_touch();
		return;
	}
#endif
	// ---------------------------------------------------------
	// Speaker control
	if(		(t_d.point_x[0] >= SPEAKER_X) &&\
			(t_d.point_x[0] <= (SPEAKER_X + SPEAKER_SIZE_X)) &&\
			(t_d.point_y[0] >= SPEAKER_Y) &&\
			(t_d.point_y[0] <= (SPEAKER_Y + SPEAKER_SIZE_Y))
	  )
	{
		ui_controls_volume_touch();
		return;
	}

	// ---------------------------------------------------------
	// S-meter control
	if(		(t_d.point_x[0] >= S_METER_X) &&\
			(t_d.point_x[0] <= (S_METER_X + S_METER_SIZE_X)) &&\
			(t_d.point_y[0] >= S_METER_Y) &&\
			(t_d.point_y[0] <= (S_METER_Y + S_METER_SIZE_Y))
	  )
	{
		ui_controls_smeter_touch();
		return;
	}

	// ---------------------------------------------------------
	// Spectrum control
	if(		(t_d.point_x[0] >= SW_FRAME_X) &&\
			(t_d.point_x[0] <= (SW_FRAME_X + SW_CONTROL_X_SIZE)) &&\
			(t_d.point_y[0] >= SW_FRAME_Y) &&\
			(t_d.point_y[0] <= (SW_FRAME_Y + SW_CONTROL_Y_SIZE))
	  )
	{
		ui_controls_spectrum_touch();
		return;
	}

	// ---------------------------------------------------------
	// Filter control
	if(		(t_d.point_x[0] >= FILTER_X) &&\
			(t_d.point_x[0] <= (FILTER_X + FILTER_SIZE_X)) &&\
			(t_d.point_y[0] >= FILTER_Y) &&\
			(t_d.point_y[0] <= (FILTER_Y + FILTER_SIZE_Y))
	  )
	{
		ui_controls_filter_touch();
		return;
	}
}
#endif

//*----------------------------------------------------------------------------
//* Function Name       : ui_driver_emwin_init
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void ui_driver_emwin_init(void)
{
	int xSize, ySize;

	// UI init
	GUI_Init();
	GUI_X_InitOS();
	WM_MULTIBUF_Enable(1);

	// Set default layer
	GUI_SetLayerVisEx (1, 0);
	//GUI_SelectLayer(0);

	// Specials
	GUI_EnableAlpha(1);
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	//BUTTON_SetReactOnLevel();

	// Get display dimension
	xSize = LCD_GetXSize();
	ySize = LCD_GetYSize();

	// Limit desktop window to display size
	WM_SetSize(WM_HBKWIN, xSize, ySize);
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_driver_task
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
//uchar wd_skip = 0;
void ui_proc_task(void const *arg)
{
	printf("ui driver start\r\n");

	// Default driver state
	ui_s.req_state 				= MODE_DESKTOP;
	ui_s.cur_state 				= MODE_DESKTOP;
	ui_s.lock_requests			= 0;

	// Init graphics lib
	ui_driver_emwin_init();

	// Set AGC, don't care what is the DSP state, just set it here
	//tsu.agc_state 	= READ_EEPROM(EEP_AGC_STATE);
	//tsu.rf_gain		= 50;
	//hw_dsp_eep_set_agc_mode(tsu.agc_state);

	// Menu items
	k_ModuleInit();
	k_ModuleAdd(&dsp_s);				// Standard DSP Menu
	k_ModuleAdd(&dsp_e);				// Extended DSP Menu
	k_ModuleAdd(&user_i);				// User Interface
	k_ModuleAdd(&clock);				// Clock Settings
	//k_ModuleAdd(&file_b);				// File Browser
	k_ModuleAdd(&reset);				// Factory Reset
	//k_ModuleAdd(&wsjt);					// WSJT-X Tools
	//k_ModuleAdd(&logbook);				// Logbook
	k_ModuleAdd(&info);					// System Information

	// Prepare Desktop screen
	if(ui_s.cur_state == MODE_DESKTOP)
	{
		// Init controls - needs to be here!
		ui_driver_init_desktop();
	}

	// Prepare menu screen
	if(ui_s.cur_state == MODE_MENU)
	{
		GUI_SetBkColor(GUI_LIGHTBLUE);
		GUI_Clear();

		ui_set_gui_profile();
		ui_init_menu();

		GUI_Exec();
	}

ui_proc_loop:

	// Touch events to correct control
	//ui_driver_touch_router();

	// Process mode change requests
	ui_driver_change_mode();

	// Refresh screen
	switch(ui_s.cur_state)
	{
		case MODE_DESKTOP:
		{
			WM_InvalidateWindow(WM_HBKWIN);
			//GUI_Exec();


			GUI_Delay(UI_REFRESH_25HZ);
			break;
		}

		case MODE_MENU:
		{
			GUI_Delay(UI_REFRESH_25HZ);
			break;
		}

		case MODE_SIDE_ENC_MENU:
		case MODE_QUICK_LOG:
		case MODE_DESKTOP_FT8:
		{
			GUI_Exec();
			GUI_Delay(100);
			break;
		}

		default:
			GUI_Delay(500);
			break;
	}

	goto ui_proc_loop;
}

#endif
