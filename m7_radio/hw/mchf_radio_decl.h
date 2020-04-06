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

#ifndef __MCHF_RADIO_DECL_H
#define __MCHF_RADIO_DECL_H

// -----------------------------------------------------------------------------------
//
#define START_UP_AUDIO_VOL			0
//
// -----------------------------------------------------------------------------------
//
#if 1
#define 	T_STEP_1HZ						1
#define 	T_STEP_10HZ						10
#define 	T_STEP_100HZ					100
#define 	T_STEP_1KHZ						1000
#define 	T_STEP_10KHZ					10000
#define 	T_STEP_100KHZ					100000
#define		T_STEP_1MHZ						1000000		// Used for transverter offset adjust
#define		T_STEP_10MHZ					10000000	// Used for transverter offset adjust
//
enum {
	T_STEP_1HZ_IDX = 0,
	T_STEP_10HZ_IDX,
	T_STEP_100HZ_IDX,
	T_STEP_1KHZ_IDX,
	T_STEP_10KHZ_IDX,
	T_STEP_100KHZ_IDX,
	T_STEP_1MHZ_IDX,
	T_STEP_10MHZ_IDX,
	T_STEP_MAX_STEPS
};
#endif
//
// -----------------------------------------------------------------------------------
//
#define DEMOD_USB					0
#define DEMOD_LSB					1
#define DEMOD_CW					2
#define DEMOD_AM					3
#define DEMOD_DIGI					4
#define DEMOD_MAX_MODE				4
//
// -----------------------------------------------------------------------------------
// Total bands supported
//
#if 0
#define	MIN_BANDS					0		// lowest band number
#define	MAX_BANDS					9		// Highest band number:  9 = General coverage (RX only) band
//
#define	KHZ_MULT					1000				// multiplier to convert oscillator frequency or band size to display kHz, used below
#define	BAND_MODE_80				0
#define	BAND_FREQ_80				3500*KHZ_MULT		// 3500 kHz
#define	BAND_SIZE_80				500*KHZ_MULT		// 500 kHz in size (Region 2)
//
#define	BAND_MODE_60				1
#define	BAND_FREQ_60				5258*KHZ_MULT		// 5258 kHz
#define	BAND_SIZE_60				150*KHZ_MULT		// 150 kHz in size to allow different allocations
//
#define	BAND_MODE_40				2
#define	BAND_FREQ_40				7000*KHZ_MULT		// 7000 kHz
#define	BAND_SIZE_40				300*KHZ_MULT		// 300 kHz in size (Region 2)
//
#define	BAND_MODE_30				3
#define	BAND_FREQ_30				10100*KHZ_MULT		// 10100 kHz
#define	BAND_SIZE_30				50*KHZ_MULT			// 50 kHz in size
//
#define	BAND_MODE_20				4
#define	BAND_FREQ_20				14000*KHZ_MULT		// 14000 kHz
#define	BAND_SIZE_20				350*KHZ_MULT		// 350 kHz in size
//
#define	BAND_MODE_17				5
#define	BAND_FREQ_17				18068*KHZ_MULT		// 18068 kHz
#define	BAND_SIZE_17				100*KHZ_MULT		// 100 kHz in size
//
#define	BAND_MODE_15				6
#define	BAND_FREQ_15				21000*KHZ_MULT		// 21000 kHz
#define	BAND_SIZE_15				450*KHZ_MULT		// 450 kHz in size
//
#define	BAND_MODE_12				7
#define	BAND_FREQ_12				24890*KHZ_MULT		// 24890 kHz
#define	BAND_SIZE_12				100*KHZ_MULT		// 100 kHz in size
//
#define	BAND_MODE_10				8
#define	BAND_FREQ_10				28000*KHZ_MULT		// 28000 kHz
#define	BAND_SIZE_10				1700*KHZ_MULT		// 1700 kHz in size
//
#define	BAND_MODE_GEN				9					// General Coverage
#define	BAND_FREQ_GEN				10000*KHZ_MULT		// 10000 kHz
#define	BAND_SIZE_GEN				1*KHZ_MULT			// Dummy variable
#endif
#if 0
enum	{
	AUDIO_300HZ = 0,
	AUDIO_500HZ,
	AUDIO_1P8KHZ,
	AUDIO_2P3KHZ,
	AUDIO_3P6KHZ,
	AUDIO_WIDE
};
#endif

// Virtual eeprom locations
#define	EEP_BASE_ADDR				0x000
#define	EEP_AUDIO_VOL				0x001
#define	EEP_CURR_BAND				0x002
#define	EEP_DEMOD_MOD				0x003
#define	EEP_CURFILTER				0x004
//
#define	EEP_SW_SMOOTH				0x005
#define	EEP_AN_MET_ON				0x006
#define	EEP_KEYER_ON				0x007
#define	EEP_AGC_STATE				0x008
//
#define	EEP_BANDS					0xE10		// pos 3600, band info, 400 bytes

#endif
