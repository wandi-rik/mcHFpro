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

#include "mchf_board.h"
#include "version.h"

#include "icc_proc.h"

//#include <stdio.h>

// Audio Driver
//#include "audio_driver.h"
//#include "cw_gen.h"

// UI Driver
//#include "ui_driver.h"
//#include "ui_rotary.h"
//#include "ui_lcd_hy28.h"

// Keyboard Driver
//#include "keyb_driver.h"

//#include "api_dsp.h"

// Misc
//#include "softdds.h"

#define HSEM_ID_0 (0U)

// Transceiver state public structure
__IO TransceiverState ts;

//*----------------------------------------------------------------------------
//* Function Name       : TransceiverStateInit
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void TransceiverStateInit(void)
{
#if 0
	// Defaults always
	ts.txrx_mode 		= TRX_MODE_RX;				// start in RX
	ts.samp_rate		= I2S_AudioFreq_48k;		// set sampling rate

	ts.enc_one_mode 	= ENC_ONE_MODE_AUDIO_GAIN;
	ts.enc_two_mode 	= ENC_TWO_MODE_RF_GAIN;
	ts.enc_thr_mode		= ENC_THREE_MODE_RIT;

	ts.band		  		= BAND_MODE_20;				// band from eeprom
	ts.band_change		= 0;						// used in muting audio during band change
	ts.filter_band		= 0;						// used to indicate the bpf filter selection for power detector coefficient selection
	ts.dmod_mode 		= DEMOD_USB;				// demodulator mode
	ts.audio_gain		= DEFAULT_AUDIO_GAIN;		// Set initial volume
	ts.audio_gain		= MAX_VOLUME_DEFAULT;		// Set max volume default
	ts.audio_gain_active = 1;						// this variable is used in the active RX audio processing function
	ts.rf_gain			= DEFAULT_RF_GAIN;			//
	ts.max_rf_gain		= MAX_RF_GAIN_DEFAULT;		// setting for maximum gain (e.g. minimum S-meter reading)
	ts.rf_codec_gain	= DEFAULT_RF_CODEC_GAIN_VAL;	// Set default RF gain (0 = lowest, 8 = highest, 9 = "Auto")
	ts.rit_value		= 0;						// RIT value
	ts.agc_mode			= AGC_DEFAULT;				// AGC setting
	ts.agc_custom_decay	= AGC_CUSTOM_DEFAULT;		// Default setting for AGC custom setting - higher = slower
	ts.filter_id		= AUDIO_DEFAULT_FILTER;		// startup audio filter
	ts.filter_300Hz_select	= FILTER_300HZ_DEFAULT;	// Select 750 Hz center filter as default
	ts.filter_500Hz_select	= FILTER_500HZ_DEFAULT;	// Select 750 Hz center filter as default
	ts.filter_1k8_select	= FILTER_1K8_DEFAULT;	// Select 1425 Hz center filter as default
	ts.filter_2k3_select	= FILTER_2K3_DEFAULT;	// Select 1412 Hz center filter as default
	ts.filter_3k6_select	= FILTER_3K6_DEFAULT;	// This is enabled by default
	ts.filter_wide_select	= FILTER_WIDE_DEFAULT;	// This is enabled by default
	//
	ts.st_gain			= DEFAULT_SIDETONE_GAIN;	// Sidetone gain
	ts.keyer_mode		= CW_MODE_IAM_B;			// CW keyer mode
	ts.keyer_speed		= DEFAULT_KEYER_SPEED;		// CW keyer speed
	ts.sidetone_freq	= CW_SIDETONE_FREQ_DEFAULT;	// CW sidetone and TX offset frequency
	ts.paddle_reverse	= 0;						// Paddle defaults to NOT reversed
	ts.cw_rx_delay		= CW_RX_DELAY_DEFAULT;		// Delay of TX->RX turnaround
	ts.unmute_delay_count		= SSB_RX_DELAY;		// Used to time TX->RX delay turnaround
	//
	ts.nb_setting		= 0;						// Noise Blanker setting
	//
	ts.tx_iq_lsb_gain_balance 	= 0;				// Default settings for RX and TX gain and phase balance
	ts.tx_iq_usb_gain_balance 	= 0;
	ts.tx_iq_lsb_gain_balance 	= 0;
	ts.tx_iq_usb_gain_balance 	= 0;
	ts.rx_iq_lsb_gain_balance = 0;
	ts.rx_iq_usb_gain_balance = 0;
	ts.rx_iq_lsb_phase_balance = 0;
	ts.rx_iq_usb_phase_balance = 0;
	//
	ts.tune_freq		= 0;
	ts.tune_freq_old	= 0;
	//
//	ts.calib_mode		= 0;						// calibrate mode
	ts.menu_mode		= 0;						// menu mode
	ts.menu_item		= 0;						// menu item selection
	ts.menu_var			= 0;						// menu item change variable
	ts.menu_var_changed	= 0;						// TRUE if a menu variable was changed and that an EEPROM save should be done

	//ts.txrx_lock		= 0;						// unlocked on start
	ts.audio_unmute		= 0;						// delayed un-mute not needed
	ts.buffer_clear		= 0;						// used on return from TX to purge the audio buffers

	ts.tx_audio_source	= TX_AUDIO_MIC;				// default source is microphone
	ts.tx_mic_gain		= MIC_GAIN_DEFAULT;			// default microphone gain
	ts.tx_mic_gain_mult	= ts.tx_mic_gain;			// actual operating value for microphone gain
	ts.mic_boost		= 0;
	ts.tx_line_gain		= LINE_GAIN_DEFAULT;		// default line gain

	ts.tune				= 0;						// reset tuning flag

	ts.tx_power_factor	= 0.50;						// TX power factor

	ts.pa_bias			= DEFAULT_PA_BIAS;			// Use lowest possible voltage as default
	ts.pa_cw_bias		= DEFAULT_PA_BIAS;			// Use lowest possible voltage as default (nonzero sets separate bias for CW mode)
	ts.freq_cal			= 0;							// Initial setting for frequency calibration
	ts.power_level		= PA_LEVEL_DEFAULT;			// See mchf_board.h for setting
	//
//	ts.codec_vol		= 0;						// Holder for codec volume
//	ts.codec_mute_state	= 0;						// Holder for codec mute state
//	ts.codec_was_muted = 0;							// Indicator that codec *was* muted
	//
	ts.powering_down	= 0;						// TRUE if powering down
	//
	ts.scope_speed		= SPECTRUM_SCOPE_SPEED_DEFAULT;	// default rate of spectrum scope update

	ts.waterfall_speed	= WATERFALL_SPEED_DEFAULT_SPI;		// default speed of update of the waterfall for parallel displays
	//
	ts.scope_filter		= SPECTRUM_SCOPE_FILTER_DEFAULT;	// default filter strength for spectrum scope
	ts.scope_trace_colour	= SPEC_COLOUR_TRACE_DEFAULT;	// default colour for the spectrum scope trace
	ts.scope_grid_colour	= SPEC_COLOUR_GRID_DEFAULT;		// default colour for the spectrum scope grid
	ts.scope_grid_colour_active = Grid;
	ts.scope_centre_grid_colour = SPEC_COLOUR_GRID_DEFAULT;		// color of center line of scope grid
	ts.scope_centre_grid_colour_active = Grid;
	ts.scope_scale_colour	= SPEC_COLOUR_SCALE_DEFAULT;	// default colour for the spectrum scope frequency scale at the bottom
	ts.scope_agc_rate	= SPECTRUM_SCOPE_AGC_DEFAULT;		// load default spectrum scope AGC rate
	ts.spectrum_db_scale = DB_DIV_10;					// default to 10dB/division
	//
	ts.menu_item		= 0;						// start out with a reasonable menu item
	//
	ts.radio_config_menu_enable = 0;				// TRUE if radio configuration menu is to be enabled
	//
	ts.cat_mode_active	= 0;						// TRUE if CAT mode is active
	//
	ts.xverter_mode		= 0;						// TRUE if transverter mode is active (e.g. offset of display)
	ts.xverter_offset	= 0;						// Frequency offset in transverter mode (added to frequency display)
	//
	ts.refresh_freq_disp	= 1;					// TRUE if frequency/color display is to be refreshed when next called - NORMALLY LEFT AT 0 (FALSE)!!!
													// This is NOT reset by the LCD function, but must be enabled/disabled externally
	//
	ts.pwr_80m_5w_adj	= 1;
	ts.pwr_60m_5w_adj	= 1;
	ts.pwr_40m_5w_adj	= 1;
	ts.pwr_30m_5w_adj	= 1;
	ts.pwr_20m_5w_adj	= 1;
	ts.pwr_17m_5w_adj	= 1;
	ts.pwr_15m_5w_adj	= 1;
	ts.pwr_12m_5w_adj	= 1;
	ts.pwr_10m_5w_adj	= 1;
	//
	ts.filter_cw_wide_disable		= 0;			// TRUE if wide filters are to be disabled in CW mode
	ts.filter_ssb_narrow_disable	= 0;			// TRUE if narrow (CW) filters are to be disabled in SSB mdoe
	ts.am_mode_disable				= 0;			// TRUE if AM mode is to be disabled
	//
	ts.tx_meter_mode	= METER_SWR;
	//
	ts.alc_decay		= ALC_DECAY_DEFAULT;		// ALC Decay (release) default value
	ts.alc_decay_var	= ALC_DECAY_DEFAULT;		// ALC Decay (release) default value
	ts.alc_tx_postfilt_gain		= ALC_POSTFILT_GAIN_DEFAULT;	// Post-filter added gain default (used for speech processor/ALC)
	ts.alc_tx_postfilt_gain_var		= ALC_POSTFILT_GAIN_DEFAULT;	// Post-filter added gain default (used for speech processor/ALC)
	ts.tx_comp_level	= 0;		// 0=Release Time/Pre-ALC gain manually adjusted, >=1:  Value calculated by this parameter
	//
	ts.freq_step_config		= 0;			// disabled both marker line under frequency and swapping of STEP buttons
	//
	ts.nb_disable		= 0;				// TRUE if noise blanker is to be disabled
	//
	ts.dsp_active		= 0;				// TRUE if DSP noise reduction is to be enabled
	ts.dsp_active_toggle	= 0xff;			// used to hold the button G2 "toggle" setting.
	ts.dsp_nr_delaybuf_len = DSP_NR_BUFLEN_DEFAULT;
	ts.dsp_nr_strength	= 0;				// "Strength" of DSP noise reduction (0 = weak)
	ts.dsp_nr_numtaps 	= DSP_NR_NUMTAPS_DEFAULT;		// default for number of FFT taps for noise reduction
	ts.dsp_notch_numtaps = DSP_NOTCH_NUMTAPS_DEFAULT;	// default for number of FFT taps for notch filter
	ts.dsp_notch_delaybuf_len =	DSP_NOTCH_DELAYBUF_DEFAULT;
	ts.dsp_inhibit		= 1;				// TRUE if DSP is to be inhibited - power up with DSP disabled
	ts.dsp_inhibit_mute = 0;				// holder for "dsp_inhibit" during muting operations to allow restoration of previous state
	ts.dsp_timed_mute	= 0;				// TRUE if DSP is to be muted for a timed amount
	ts.dsp_inhibit_timing = 0;				// used to time inhibiting of DSP when it must be turned off for some reason
	ts.reset_dsp_nr		= 0;				// TRUE if DSP NR coefficients are to be reset when "audio_driver_set_rx_audio_filter()" is called
	ts.lcd_backlight_brightness = 0;		// = 0 full brightness
	ts.lcd_backlight_blanking = 0;			// MSB = 1 for auto-off of backlight, lower nybble holds time for auto-off in seconds
	//
	ts.tune_step		= 0;				// Used for press-and-hold step size changing mode
	ts.frequency_lock	= 0;				// TRUE if frequency knob is locked
	//
	ts.tx_disable		= 0;				// TRUE if transmitter is to be disabled
	ts.misc_flags1		= 0;				// Used to hold individual status flags, stored in EEPROM location "EEPROM_MISC_FLAGS1"
	ts.misc_flags2		= 0;				// Used to hold individual status flags, stored in EEPROM location "EEPROM_MISC_FLAGS2"
	ts.sysclock			= 0;				// This counts up from zero when the unit is powered up at precisely 100 Hz over the long term.  This
											// is NEVER reset and is used for timing certain events.
	ts.version_number_release	= 0;		// version release - used to detect firmware change
	ts.version_number_build = 0;			// version build - used to detect firmware change
	ts.nb_agc_time_const	= 0;			// used to calculate the AGC time constant
	ts.cw_offset_mode	= 0;				// CW offset mode (USB, LSB, etc.)
	ts.cw_lsb			= 0;				// Flag that indicates CW operates in LSB mode when TRUE
	ts.iq_freq_mode		= 0;				// used to set/configure the I/Q frequency/conversion mode
	ts.conv_sine_flag	= 0;				// FALSE until the sine tables for the frequency conversion have been built (normally zero, force 0 to rebuild)
	ts.lsb_usb_auto_select	= 0;			// holds setting of LSB/USB auto-select above/below 10 MHz
	ts.hold_off_spectrum_scope	= 0;		// this is a timer used to hold off updates of the spectrum scope when an SPI LCD display interface is used
	ts.lcd_blanking_time = 0;				// this holds the system time after which the LCD is blanked - if blanking is enabled
	ts.lcd_blanking_flag = 0;				// if TRUE, the LCD is blanked completely (e.g. backlight is off)
	ts.freq_cal_adjust_flag = 0;			// set TRUE if frequency calibration is in process
	ts.xvtr_adjust_flag = 0;				// set TRUE if transverter offset adjustment is in process
	ts.rx_muting = 0;						// set TRUE if audio output is to be muted
	ts.rx_blanking_time = 0;				// this is a timer used to delay the un-blanking of the audio after a large synthesizer tuning step
	ts.vfo_mem_mode = 0;					// this is used to record the VFO/memory mode (0 = VFO "A" = backwards compatibility)
											// LSB+6 (0x40) = 0:  VFO A,  1 = VFO B
											// LSB+7 (0x80) = 0:  Normal mode, 1 = SPLIT mode
											// Other bits are currently reserved
	ts.voltmeter_calibrate	= POWER_VOLTMETER_CALIBRATE_DEFAULT;	// Voltmeter calibration constant
	ts.thread_timer = 0;					// used to time thread
	ts.waterfall_color_scheme = WATERFALL_COLOR_DEFAULT;	// color scheme for waterfall display
	ts.waterfall_vert_step_size = WATERFALL_STEP_SIZE_DEFAULT;		// step size in waterfall display
	ts.waterfall_offset = WATERFALL_OFFSET_DEFAULT;		// Offset for waterfall display (brightness)
	ts.waterfall_contrast = WATERFALL_CONTRAST_DEFAULT;	// contrast setting for waterfall display
	ts.spectrum_scope_scheduler = 0;		// timer for scheduling the next update of the spectrum scope update
	ts.spectrum_scope_nosig_adjust = SPECTRUM_SCOPE_NOSIG_ADJUST_DEFAULT;	// Adjustment for no signal adjustment conditions for spectrum scope
	ts.waterfall_nosig_adjust = WATERFALL_NOSIG_ADJUST_DEFAULT;		// Adjustment for no signal adjustment conditions for waterfall
	ts.waterfall_size	= WATERFALL_SIZE_DEFAULT;		// adjustment for waterfall size
	ts.fft_window_type = FFT_WINDOW_DEFAULT;			// FFT Windowing type
	ts.dvmode = 0;							// disable "DV" mode RX/TX functions by default
	ts.tx_audio_muting_timing = 0;			// timing value used for muting TX audio when keying PTT to suppress "click" or "thump"
	ts.tx_audio_muting_timer = 0;			// timer used for muting TX audio when keying PTT to suppress "click" or "thump"
	ts.tx_audio_muting_flag = 0;			// when TRUE, audio is to be muted after PTT/keyup
	ts.filter_disp_colour = FILTER_DISP_COLOUR_DEFAULT;	//
	ts.vfo_mem_flag = 0;					// when TRUE, memory mode is enabled
	ts.mem_disp = 0;						// when TRUE, memory display is enabled
#endif
}

#if 0

//*----------------------------------------------------------------------------
//* Function Name       : MiscInit
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void MiscInit(void)
{
	//printf("misc init...\n\r");

	// Init Soft DDS
	softdds_setfreq(0.0,ts.samp_rate,0);
	//softdds_setfreq(500.0,ts.samp_rate,0);
	//softdds_setfreq(1000.0,ts.samp_rate,0);
	//softdds_setfreq(2000.0,ts.samp_rate,0);
	//softdds_setfreq(3000.0,ts.samp_rate,0);
	//softdds_setfreq(4000.0,ts.samp_rate,0);

	//printf("misc init ok\n\r");
}

#if 0
static void wd_reset(void)
{
	// Init WD
	if(!wd_init_enabled)
	{
		// Start watchdog
		WWDG_Enable(WD_REFRESH_COUNTER);

		// Reset
		wd_init_enabled = 1;
		TimingDelay 	= 0;

		return;
	}

	// 40mS flag for WD reset
	if(TimingDelay > 40)
	{
		TimingDelay = 0;
		//GREEN_LED_PIO->ODR ^= RED_LED;

		// Update WWDG counter
		WWDG_SetCounter(WD_REFRESH_COUNTER);
	}
}
#endif

#endif

//*----------------------------------------------------------------------------
//* Function Name       : CriticalError
//* Object              : should never be here, really
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void CriticalError(unsigned long error)
{
	NVIC_SystemReset();
}

//*----------------------------------------------------------------------------
//* Function Name       : NMI_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void NMI_Handler(void)
{
	//printf("NMI_Handler called\n\r");
	CriticalError(1);
}

//*----------------------------------------------------------------------------
//* Function Name       : HardFault_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void HardFault_Handler(void)
{
	//printf("HardFault_Handler called\n\r");
	CriticalError(2);
}

//*----------------------------------------------------------------------------
//* Function Name       : MemManage_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void MemManage_Handler(void)
{
	//printf("MemManage_Handler called\n\r");
	CriticalError(3);
}

//*----------------------------------------------------------------------------
//* Function Name       : BusFault_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void BusFault_Handler(void)
{
	//printf("BusFault_Handler called\n\r");
	CriticalError(4);
}

//*----------------------------------------------------------------------------
//* Function Name       : UsageFault_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void UsageFault_Handler(void)
{
	//printf("UsageFault_Handler called\n\r");
	CriticalError(5);
}

//*----------------------------------------------------------------------------
//* Function Name       : SVC_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void SVC_Handler(void)
{
	//printf("SVC_Handler called\n\r");
	CriticalError(6);
}

//*----------------------------------------------------------------------------
//* Function Name       : DebugMon_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void DebugMon_Handler(void)
{
	//printf("DebugMon_Handler called\n\r");
	CriticalError(7);
}

//volatile uint32_t Notif_Recieved;

void SysTick_Handler(void)
{
	HAL_IncTick();
}

void HSEM2_IRQHandler(void)
{
	HAL_HSEM_IRQHandler();
}

extern void printf_init(unsigned char is_shared);

//*----------------------------------------------------------------------------
//* Function Name       : main
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
int main(void)
{
	// Configure the Cortex-M4 ART Base address to D2_AXISRAM_BASE : 0x10000000
    __HAL_ART_CONFIG_BASE_ADDRESS(D2_AXISRAM_BASE);

    // HW semaphore Clock enable
    __HAL_RCC_HSEM_CLK_ENABLE();

    // Activate HSEM notification for Cortex-M4
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

	// Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
	//    perform system initialization (system clock config, external memory configuration.. )
    HAL_PWREx_ClearPendingEvent();
    HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);

	HAL_Init();

	// Init debug print in shared mode
	printf_init(1);

	// Clear Flags generated during the wakeup notification
	HSEM_COMMON->ICR |= ((uint32_t)__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
	HAL_NVIC_ClearPendingIRQ(HSEM2_IRQn);

	// Init allocated leds
	BSP_LED_Init(LED_RED);
	BSP_LED_Init(LED_BLUE);

	// Set default transceiver state
	TransceiverStateInit();

	// Core power on
	BSP_LED_On(LED_RED);

	printf("--------------------------------------\r\n");
    printf("--- Debug Print Session on (dsp) ---\r\n");
	printf("Firmware v: %d.%d.%d.%d\r\n", MCHF_D_VER_MAJOR, MCHF_D_VER_MINOR, MCHF_D_VER_RELEASE, MCHF_D_VER_BUILD);

	// ICC driver init
	icc_proc_hw_init();

	while (1)
	{
		// ICC driver process
		icc_proc_task(NULL);
	}
}

#if 0
//*----------------------------------------------------------------------------
//* Function Name       : main
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
int main(void)
{

	// HW init
//!	mchf_board_init();

	// Power on
//!	mchf_board_green_led(1);

	// Set default transceiver state
//!	TransceiverStateInit();

	// Virtual Eeprom init
	//ts.ee_init_stat = EE_Init();	// get status of EEPROM initialization

	// Show logo
	//UiLcdHy28_ShowStartUpScreen(100);

	// Extra init
//!	MiscInit();

	// Init the RX Hilbert transform/filter prior
	// to initializing the audio!
	//
//!	UiCalcRxPhaseAdj();
	//
	// Init TX Hilbert transform/filter
	//
//!	UiCalcTxPhaseAdj();	//

//!	UiDriverLoadFilterValue();	// Get filter value so we can init audio with it

	// Audio HW init
//!	audio_driver_init();

	// Usb Host driver init
	//keyb_driver_init();

	// UI HW init
//!	ui_driver_init();

	// Audio HW init - again, using EEPROM-loaded values
//!	audio_driver_init();
	//
//!	ts.audio_gain_change = 99;		// Force update of volume control
//!	uiCodecMute(0);					// make cure codec is un-muted

	#ifdef DSP_MODE
	// Api driver - reuse the SPI from LCD
//!	api_dsp_init();
	#endif

	#ifdef DEBUG_BUILD
	printf("== main loop starting ==\n\r");
	#endif

	// Transceiver main loop
	for(;;)
	{
		// UI events processing
//!		ui_driver_thread();

		// Audio driver processing
		//audio_driver_thread();

		#ifdef DSP_MODE
		// talk to UI board
//!		api_dsp_thread();
		#endif

		// USB Host driver processing
		//usbh_driver_thread();

		// Reset WD - not working
		//wd_reset();

		//----------printf("board revision: %d\n\r",get_pcb_rev());
	}
}
#endif
