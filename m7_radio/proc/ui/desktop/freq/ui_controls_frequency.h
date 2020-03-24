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
#ifndef UI_CONTROLS_FREQUENCY
#define UI_CONTROLS_FREQUENCY

//#define FRAME_MAIN_DIAL
#define FREQ_ENABLE_SECOND

//#define FREQ_REPAINT_FIRST			0x01
//#define FREQ_REPAINT_SECOND			0x02

#define FREQ_FONT					GUI_FONT_D24X32

#define FREQ_FONT_SIZE_X			24
#define FREQ_FONT_SIZE_Y			32

// VFO B decl
#define FREQ_FONT_SIZE1_X			12
#define FREQ_FONT_SIZE1_Y			17
//
#define VFO_B_SEG_SEL_COLOR			0x00E0E0E0
#define VFO_B_SEG_OFF_COLOR			GUI_LIGHTGRAY
#define VFO_B_SEG_ON_COLOR			GUI_WHITE

// Frequency public structure
typedef struct DialFrequency
{
	uchar	last_active_vfo;
	ulong	last_screen_step;

	// VFO A
	ulong 	vfo_a_scr_freq;
	uchar	vfo_a_segments_invalid;
	//
	uchar	dial_100_mhz;
	uchar	dial_010_mhz;
	uchar	dial_001_mhz;
	uchar	dial_100_khz;
	uchar	dial_010_khz;
	uchar	dial_001_khz;
	uchar	dial_100_hz;
	uchar	dial_010_hz;
	uchar	dial_001_hz;

	// VFO B
	ulong 	vfo_b_scr_freq;
	uchar	vfo_b_segments_invalid;
	//
	uchar	sdial_100_mhz;
	uchar	sdial_010_mhz;
	uchar	sdial_001_mhz;
	uchar	sdial_100_khz;
	uchar	sdial_010_khz;
	uchar	sdial_001_khz;
	uchar	sdial_100_hz;
	uchar	sdial_010_hz;
	uchar	sdial_001_hz;

} DialFrequency;

// Exports
void ui_controls_frequency_init(void);
void ui_controls_frequency_quit(void);

void ui_controls_frequency_touch(void);
void ui_controls_frequency_refresh(uchar flags);

void ui_controls_frequency_change_band(void);

//void ui_controls_frequency_rebuild_second(void);

#endif
