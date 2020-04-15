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
#ifndef __MCHF_PRO_BOARD_H
#define __MCHF_PRO_BOARD_H

#define USE_DSP_CORE
//
#include "mchf_types.h"
#include "mchf_radio_decl.h"
#include "cmsis_os.h"
//
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//
// To measure OS performance
#include "cpu_utils.h"
//
// Eeprom emulation
#include "virt_eeprom.h"

#include "mchf_icc_def.h"
//
// What board are we running on ?
//
// - need this in final release at all ?
//
//#define BOARD_DISCO
#define BOARD_MCHF
//
#define PCB_REV_26JULY18
//
// -----------------------------------------------------------------------------
// Are we using internal CPU RAM or external SDRAM
//
// - bad soldering/connection with SDRAM will cause the emWin to hang in GUI_init()
// - might be safe to boot up with a simple UI, run some HW tests, then do full boot
//
//#define USE_INT_RAM
#define USE_SDRAM
//
#define	EMWIN_RAM_SIZE 				(1024*1024)
//
// -----------------------------------------------------------------------------
// Oscillators configuration
//
// - Main
#define INT_16MHZ					0
#define EXT_16MHZ_XTAL				1
#define EXT_16MHZ_TCXO				2
// - Real Time Clock
#define INT_32KHZ					0
#define EXT_32KHZ_XTAL				1
//
//#define HSE_VALUE    				((uint32_t)16000000)
//#define HSI_VALUE    				((uint32_t)16000000)
#define CSI_VALUE    				((uint32_t)4000000)
//
//#define VECT_TAB_OFFSET  			0x00				// alpha stage, base of flash
//#define VECT_TAB_OFFSET  			0x20000				// sector 1, we have bootloader at base
//
// -----------------------------------------------------------------------------
//
#define		CHIP_UNIQUE_ID			0x1FF1E800
#define		CHIP_FLS_SIZE			0x1FF1E880
//
// -----------------------------------------------------------------------------
// Some better looking OS macros
//
#define EnterCriticalSection		taskENTER_CRITICAL
#define ExitCriticalSection			taskEXIT_CRITICAL
//#define OsDelayMs					osDelay
//
// -----------------------------------------------------------------------------
// Drivers start delays
// - start the UI driver first, the rest with delay, so UI can be painted first
//
#define BOOT_UP_LCD_DELAY			2000
//
#define SIGNAL_DRV_START_DELAY		500
#define TOUCHC_DRV_START_DELAY		1000
#define KEYPAD_DRV_START_DELAY		1500
#define KEYLED_DRV_START_DELAY		2000
//
// -----------------------------------------------------------------------------
// Firmware execution context
// if this project becomes community effort eventually, it makes sense to mark
// each function with the task context executes into, honestly will save a lot
// of future headache!
//
// Nothing much here except quick initial stack setup, basic hw init, MMU On
// clocks and start the OS
#define CONTEXT_RESET_VECTOR
//
#define CONTEXT_IRQ_SYS_TICK
//
#define CONTEXT_IRQ_LTDC
//
#define CONTEXT_DRIVER_UI
//
//#define CONTEXT_DRIVER_KEYPAD
//
//#define CONTEXT_DRIVER_DIGITIZER
//
// Encoders
#define CONTEXT_ROTARY
//
//#define CONTEXT_SD
//
// Inter-processor communication: STM32 <-> ESP32
#define CONTEXT_IPC_PROC
//
// Core to core communication: M7 <-> M4
#define CONTEXT_ICC
//
//
// Touch screen
#define CONTEXT_TOUCH
//
// -----------------------------------------------------------------------------
//
#define IPC_PROC_START_DELAY 	5000


//
//#define H7_200MHZ
#define H7_400MHZ
//
// -----------------------------------------------------------------------------
// Nice facility for testing the analogue s-meter via side encoder
//
//#define USE_SIDE_ENC_FOR_S_METER
//
// -----------------------------------------------------------------------------
//
#define	VFO_A					0
#define	VFO_B					1
//
// -----------------------------------------------------------------------------
// Public UI driver state
#define	MODE_DESKTOP			0
#define MODE_MENU				1
#define MODE_SIDE_ENC_MENU		2
#define MODE_DESKTOP_FT8		3
#define MODE_QUICK_LOG			4
//
struct UI_DRIVER_STATE {
	// Current LCD state - Desktop or Menu
	// Request flag, updated by keypad driver, read only for the UI task
	uchar	req_state;
	// Accepted state by the UI driver(read/write local)
	uchar	cur_state;
	// Flag to lock out keyboard driver request updates while GUI repaints
	uchar	lock_requests;
	//
	uchar	dummy;
	//
} UI_DRIVER_STATE;
//
// -----------------------------------------------------------------------------
// DSP API codes
//
#define API_UPD_FREQ					0x0001
#define API_UPD_BAND					0x0002
#define API_UPD_VOL						0x0003
#define API_UPD_DEMOD					0x0004
#define API_UPD_FILT					0x0005
#define API_UPD_STEP					0x0006
#define API_UPD_NCO						0x0007
#define API_CW_TX						0x0008
#define API_BROADCAST_MODE				0x0009
#define API_WRITE_EEP					0x000A
#define API_RESTART						0x000B
//
// The 16 bit msg id is used in the DSP handler directly
// to switch execution, the data buffer is for extra values
// So, 14 bytes might not be enough, but every task(control?) should allocate
// own static RAM copy, which reserves extra memory
// but is needed as this struct is passed in queue via pointer and
// needs to be valid while the message is propagating, so can't be temporary
// function stack that might not be valid in few uS, specially in fast
// UI redrawing routines
//
#define API_MAX_PAYLOAD					13
//
struct APIMessage {

	ushort 	usMessageID;
	uchar	ucPayload;
	uchar 	ucData[API_MAX_PAYLOAD];

} APIMessage;
// -----------------------------------------------------------------------------
// calls to DPS driver
//
#define DSP_MAX_PAYLOAD					50
//
struct DSPMessage {

	uchar 	ucMessageID;
	uchar	ucProcessDone;
	uchar	ucDataReady;
	char 	cData[DSP_MAX_PAYLOAD];

} DSPMessage;

#define	TASK_PROC_IDLE				0
#define	TASK_PROC_WORK				1
#define	TASK_PROC_DONE				2

struct ESPMessage {

	uchar 	ucMessageID;
	uchar	ucProcStatus;
	uchar	ucDataReady;
	uchar	ucExecResult;

	ushort	usPayloadSize;

	uchar 	ucData[128];

} ESPMessage;

// -----------------------------------------------------------------------------
// Hardware regs, read before MMU init
struct CM7_CORE_DETAILS {

	// X and Y coordinates on the wafer
	ulong 	wafer_coord;
	// Bits 31:8 UID[63:40]: LOT_NUM[23:0] 	- Lot number (ASCII encoded)
	// Bits 7:0 UID[39:32]: WAF_NUM[7:0] 	- Wafer number (8-bit unsigned number)
	ulong	wafer_number;
	// Bits 31:0 UID[95:64]: LOT_NUM[55:24]	- Lot number (ASCII encoded)
	ulong	lot_number;
	// Flash size
	ushort 	fls_size;
	ushort	dummy;

} CM7_CORE_DETAILS;
//
// Per band settings
struct BAND_INFO {

	// Frequency boundary
	ulong 	band_start;
	//
	ulong	band_end;
	// Common controls
	uchar 	volume;
	uchar 	demod_mode;
	uchar 	filter;
	uchar 	dsp_mode;
	//
	ulong	step;
	// Frequency
	ulong	vfo_a;
	//
	ulong	vfo_b;
	//
	short 	nco_freq;
	uchar	fixed_mode;
	uchar   active_vfo;			// A, B
	//
	// Align four!

} BAND_INFO;
//
// Public structure to hold current radio state
struct TRANSCEIVER_STATE_UI {

	// Per band info
	struct BAND_INFO	band[MAX_BANDS];

	// -----------------------------
	// Clock to use
	uchar 	main_clk;
	// RCC clock to use
	uchar 	rcc_clk;
	ushort 	reset_reason;
	// --
	// -----------------------------
	// DSP status
	uchar 	dsp_alive;
	uchar 	dsp_seq_number_old;
	uchar 	dsp_seq_number;
	uchar 	dsp_blinker;
	// --
	uchar 	dsp_rev1;
	uchar 	dsp_rev2;
	uchar 	dsp_rev3;
	uchar 	dsp_rev4;
	// --
	ulong 	dsp_freq;
	short 	dsp_nco_freq;
	uchar 	dsp_step_idx;
	uchar 	step_idx;
	// --
	uchar 	dsp_volume;
	uchar 	dsp_filter;
	uchar   dsp_band;
	uchar 	dsp_demod;

	uchar	pcb_rev;
	// --
	// -----------------------------
	// Local status
	uchar	agc_state;
	uchar	rf_gain;
	// --
	uchar 	audio_mute_flag;
	// ---
	uchar 	curr_band;
	// --
	ulong	step;
	// --
	uchar	cw_tx_state;		// 0 - idle, 1 - on, 2 - release
	uchar	cw_iamb_type;

	uchar	eeprom_init_done;
	//uchar	eeprom_data_valid;
	//
	// Request flags for CPU->DSP comm
	uchar	update_audio_dsp_req;
	uchar	update_freq_dsp_req;
	uchar	update_band_dsp_req;
	uchar	update_demod_dsp_req;
	uchar	update_filter_dsp_req;

	uchar	update_nco_dsp_req;

	// DSP public structure update flags
	uchar	update_dsp_eep_req;
	//
	ushort	update_dsp_eep_offset;
	uchar	update_dsp_eep_size;
	ulong	update_dsp_eep_value;

	uchar	update_dsp_restart;

	int		wifi_rssi;

	// Always align last member!

} TRANSCEIVER_STATE_UI;
//
#define	SW_CONTROL_BIG				0
#define	SW_CONTROL_MID				1
#define	SW_CONTROL_SMALL			2
//
// Spectrum/Waterfall control publics
struct UI_SW {
	//
	// screen pixel data
	uchar 	fft_value[1024];
	// FFT array from DSP
	uchar 	fft_dsp[1024];
	//
	uchar 	ctrl_type;
	//
	uchar	dummy1;
	// --
	ushort 	bandpass_start;
	ushort 	bandpass_end;

} UI_SW;
//
// -----------------------------------------------------------------------------
// HAL compatibility
//#define assert_param(expr) ((void)0)
//
//typedef enum
//{
//  RESET = 0,
//  SET = !RESET
//} FlagStatus, ITStatus;
//
//typedef enum
//{
//  DISABLE = 0,
//  ENABLE = !DISABLE
//} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))
//
//typedef enum
//{
//  ERROR = 0,
//  SUCCESS = !ERROR
//} ErrorStatus;
//
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))

// -----------------------------------------------------------------------------

typedef void FAST_REFRESH(void);

// Use FreeRTOS allocator
#define malloc 					pvPortMalloc
#define free					vPortFree

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void 	mchf_pro_board_debug_led_init(void);
void 	mchf_pro_board_blink_if_alive(uchar flags);

void 	mchf_pro_board_read_cpu_details(void);
void 	mchf_pro_board_start_gpio_clocks(void);

void 	mchf_pro_board_mpu_config(void);
void 	mchf_pro_board_cpu_cache_enable(void);

uchar 	mchf_pro_board_system_clock_config(uchar clk_src);
uchar 	mchf_pro_board_rtc_clock_config(uchar clk_src);
void 	mchf_pro_board_rtc_clock_disable(void);

void 	mchf_pro_board_swo_init(void);
void 	mchf_pro_board_mco2_on(void);

void 	mchf_pro_board_sensitive_hw_init(void);

// in main.c !
void 	transceiver_init_eep_defaults(void);

void print_hex_array(uchar *pArray, ushort aSize);

#endif
