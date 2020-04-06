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
#ifndef UI_CONTROLS_LAYOUT_H
#define UI_CONTROLS_LAYOUT_H

#include "GUI.h"

#define ID_WINDOW_VOLUME          	(GUI_ID_USER + 0x00)
#define ID_BUTTON_VOLUMET          	(GUI_ID_USER + 0x01)
#define ID_BUTTON_VOLUMEM          	(GUI_ID_USER + 0x02)
#define ID_BUTTON_VOLUMEP          	(GUI_ID_USER + 0x03)

// ----------------------------------------------------------------------------
// Analogue S-meter
//
#define S_METER_X					1
#define S_METER_Y					1
//
#define S_METER_FRAME_LEFT			1
#define S_METER_FRAME_RIGHT			0
#define S_METER_FRAME_TOP			1
#define S_METER_FRAME_BOTTOM		0	// ToDo: fix bottom right corner!!
//
#define S_METER_FRAME_WIDTH			3
#define S_METER_FRAME_CURVE			0	// corner radius actually

#define S_METER_SIZE_X 				(349 + S_METER_FRAME_WIDTH)
#define S_METER_SIZE_Y 				(140 + S_METER_FRAME_WIDTH)
//
// ----------------------------------------------------------------------------
// Combined waterfall/spectrum scope control
// -- big size version
// frame position - bottom of the screen
#define SW_FRAME_X					0
#define SW_FRAME_Y					169	//214
//
#define HEADER_Y_SIZE				20
//
#define SW_FRAME_WIDTH				2
#define SW_FRAME_CORNER_R			3
//
#define FOOTER_Y_SIZE				16
//
// spectrum only position
#define SCOPE_X						(SW_FRAME_X + SW_FRAME_WIDTH)
#define SCOPE_Y						(SW_FRAME_Y + HEADER_Y_SIZE + 2)
//
// spectrum only size
#define SCOPE_X_SIZE				796
#define SCOPE_Y_SIZE				112				// ~0x6F
//
// waterfall only position
#define WATERFALL_X					SCOPE_X
#define WATERFALL_Y					(SCOPE_Y + SCOPE_Y_SIZE + 0)
//
// waterfall only size
#define WATERFALL_X_SIZE			SCOPE_X_SIZE
#define WATERFALL_Y_SIZE			157				//SCOPE_Y_SIZE
//
// smooth button
#define SMOOTH_X					((SW_FRAME_X + SW_FRAME_X_SIZE - 2)/2 + 115)
#define SMOOTH_Y					(SW_FRAME_Y + 4)
#define SMOOTH_X_SIZE				65
#define SMOOTH_Y_SIZE				15
//
// center/fixed button
#define CENTER_X					((SW_FRAME_X + SW_FRAME_X_SIZE - 2)/2 + 210)
#define CENTER_Y					(SW_FRAME_Y + 4)
#define CENTER_X_SIZE				65
#define CENTER_Y_SIZE				15
//
// Span control
#define SPAN_X						((SW_FRAME_X + SW_FRAME_X_SIZE - 2)/2 + 290)
#define SPAN_Y						(SW_FRAME_Y + 4)
#define SPAN_X_SIZE					65
#define SPAN_Y_SIZE					15
//
// Total size of control
#define SW_FRAME_X_SIZE				799
#define SW_FRAME_Y_SIZE				(HEADER_Y_SIZE + FOOTER_Y_SIZE + SCOPE_Y_SIZE + WATERFALL_Y_SIZE + SW_FRAME_WIDTH*2 + 1)
//
#define SW_CONTROL_X_SIZE			(SW_FRAME_X_SIZE + 1)
#define SW_CONTROL_Y_SIZE			(SW_FRAME_Y_SIZE + 1)
//
//
// ----------------------------------------------------------------------------
// Combined waterfall/spectrum scope control
// -- medium size version
// frame position - bottom of the screen
#define SW_FRAME_X_MID				0
#define SW_FRAME_Y_MID				214
//
#define HEADER_Y_SIZE_MID			20
//
#define SW_FRAME_WIDTH_MID			2
#define SW_FRAME_CORNER_R_MID		3
//
#define FOOTER_Y_SIZE_MID			16
//
// spectrum only position
#define SCOPE_X_MID					(SW_FRAME_X_MID + SW_FRAME_WIDTH_MID)
#define SCOPE_Y_MID					(SW_FRAME_Y_MID + HEADER_Y_SIZE_MID + 2)
//
// spectrum only size
#define SCOPE_X_SIZE_MID			672
#define SCOPE_Y_SIZE_MID			112				// ~0x6F
//
// waterfall only position
#define WATERFALL_X_MID				SCOPE_X_MID
#define WATERFALL_Y_MID				(SCOPE_Y_MID + SCOPE_Y_SIZE_MID + 0)
//
// waterfall only size
#define WATERFALL_X_SIZE_MID		SCOPE_X_SIZE_MID
#define WATERFALL_Y_SIZE_MID		SCOPE_Y_SIZE_MID
//
// smooth button
#define SMOOTH_X_MID				((SW_FRAME_X_MID + SW_FRAME_X_SIZE_MID - 2)/2 + 80)
#define SMOOTH_Y_MID				(SW_FRAME_Y_MID + 4)
#define SMOOTH_X_SIZE_MID			65
#define SMOOTH_Y_SIZE_MID			15
//
// center/fixed button
#define CENTER_X_MID				((SW_FRAME_X_MID + SW_FRAME_X_SIZE_MID - 2)/2 + 160)
#define CENTER_Y_MID				(SW_FRAME_Y_MID + 4)
#define CENTER_X_SIZE_MID			65
#define CENTER_Y_SIZE_MID			15
//
// Span control
#define SPAN_X_MID					((SW_FRAME_X_MID + SW_FRAME_X_SIZE_MID - 2)/2 + 230)
#define SPAN_Y_MID					(SW_FRAME_Y_MID + 4)
#define SPAN_X_SIZE_MID				65
#define SPAN_Y_SIZE_MID				15
//
// Total size of control
#define SW_FRAME_X_SIZE_MID			(SCOPE_X_SIZE_MID + 3)
#define SW_FRAME_Y_SIZE_MID			(HEADER_Y_SIZE_MID + FOOTER_Y_SIZE_MID + SCOPE_Y_SIZE_MID + WATERFALL_Y_SIZE_MID + SW_FRAME_WIDTH_MID*2 + 1)
//
#define SW_CONTROL_X_SIZE_MID		(SW_FRAME_X_SIZE_MID + 1)
#define SW_CONTROL_Y_SIZE_MID		(SW_FRAME_Y_SIZE_MID + 1)
//
// ----------------------------------------------------------------------------
// Iambic keyer control
//
#define IAMB_KEYER_X				678
#define IAMB_KEYER_Y				214
//
#define IAMB_KEYER_SIZE_X			121
#define IAMB_KEYER_SIZE_Y			265
//
#define IAMB_BTN_TOP_X0				(IAMB_KEYER_X + 2)
#define IAMB_BTN_TOP_X1				(IAMB_KEYER_X + IAMB_KEYER_SIZE_X   - 2)
#define IAMB_BTN_TOP_Y0				(IAMB_KEYER_Y + 2)
#define IAMB_BTN_TOP_Y1				(IAMB_KEYER_Y + IAMB_KEYER_SIZE_Y/2 - 2)
//
#define IAMB_BTN_BTM_X0				(IAMB_KEYER_X + 2)
#define IAMB_BTN_BTM_X1				(IAMB_KEYER_X + IAMB_KEYER_SIZE_X   - 2)
#define IAMB_BTN_BTM_Y0				(IAMB_KEYER_Y + IAMB_KEYER_SIZE_Y/2 + 2)
#define IAMB_BTN_BTM_Y1				(IAMB_KEYER_Y + IAMB_KEYER_SIZE_Y   - 2)
//
// ----------------------------------------------------------------------------
// Main frequency control
//
#define M_FREQ_X					531
#define M_FREQ_Y					122
//
#define M_FREQ1_X					356
#define M_FREQ1_Y					136
//
// ----------------------------------------------------------------------------
// Digitizer state control
//
#define DIGITIZER_X					600
#define DIGITIZER_Y					2
//
// ----------------------------------------------------------------------------
// Band control
#define BAND_X						696
#define BAND_Y						95
//
// ----------------------------------------------------------------------------
// Step
#define VFO_STEP_X					414
#define VFO_STEP_Y					95
//
#define VFO_STEP_SIZE_X				52
#define VFO_STEP_SIZE_Y				20
//
// ----------------------------------------------------------------------------
// RX/TX
#define RADIO_MODE1_X				360
#define RADIO_MODE1_Y				122
//
// ----------------------------------------------------------------------------
// Decoder
#define DECODER_MODE_X				356
#define DECODER_MODE_Y				95
//
// ----------------------------------------------------------------------------
// AGC control
#define AGC_X						473
#define AGC_Y						95
//
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Desktop layouts - for now via conditional flags
// ToDo: implement as const struct array declaration, to allow dynamic loading
//
//#define DESKTOP_LAYOUT_1
#define DESKTOP_LAYOUT_2

#ifdef DESKTOP_LAYOUT_1
// ----------------------------------------------------------------------------
// CPU Load control
//
#define CPU_L_X						360
#define CPU_L_Y						1
//
// ----------------------------------------------------------------------------
// DSP alive control
#define DSP_POS_X					480
#define DSP_POS_Y					1
//
// ----------------------------------------------------------------------------
// Clock control
#define CLOCK_X						620
#define CLOCK_Y						2
//
// ----------------------------------------------------------------------------
// SD Card control
#define SD_CARD_X					690
#define SD_CARD_Y					1
//
// ----------------------------------------------------------------------------
// Speaker control
#define SPEAKER_X					738
#define SPEAKER_Y					1
//
#define SPEAKER_SIZE_X				62
#define SPEAKER_SIZE_Y				50
//
// ----------------------------------------------------------------------------
// Filter
#define FILTER_X					356
#define FILTER_Y					60
//
#define FILTER_SIZE_X				438
#define FILTER_SIZE_Y				24
//
// ----------------------------------------------------------------------------
#endif
#ifdef DESKTOP_LAYOUT_2
// ----------------------------------------------------------------------------
// CPU Load control
//
#define CPU_L_X						360
#define CPU_L_Y						35
//
// ----------------------------------------------------------------------------
// DSP alive control
#define DSP_POS_X					480
#define DSP_POS_Y					35
//
// ----------------------------------------------------------------------------
// Clock control
#define CLOCK_X						540
#define CLOCK_Y						56
//
// ----------------------------------------------------------------------------
// SD Card control
#define SD_CARD_X					690
#define SD_CARD_Y					35
//
// ----------------------------------------------------------------------------
// Speaker control
#define SPEAKER_X					738
#define SPEAKER_Y					35
//
#define SPEAKER_SIZE_X				62
#define SPEAKER_SIZE_Y				50
//
// ----------------------------------------------------------------------------
// Filter
#define FILTER_X					360
#define FILTER_Y					1
//
#define FILTER_SIZE_X				438
#define FILTER_SIZE_Y				24
//
// ----------------------------------------------------------------------------
#endif

#endif
