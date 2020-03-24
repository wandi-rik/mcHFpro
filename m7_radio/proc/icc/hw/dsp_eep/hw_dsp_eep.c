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

#include "hw_dsp_eep.h"

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

//*----------------------------------------------------------------------------
//* Function Name       : hw_dsp_eep_wait_lock
//* Object              : wait for API driver process
//* Input Parameters    :
//* Output Parameters   : this func might not be thread safe, check!!!
//* Functions called    :
//*----------------------------------------------------------------------------
uchar hw_dsp_eep_wait_lock(void)
{
	ushort wait = 0;

	// Skip wait, API driver probably ready for request
	if(tsu.update_dsp_eep_req == 0)
		return 0;

	// Wait API driver
	while((tsu.update_dsp_eep_req) && (wait < 50))
	{
		printf("waiting lock release...\r\n");

		wait++;
		vTaskDelay(5);

		// If process complete, return ok
		if(tsu.update_dsp_eep_req == 0)
			return 0;
	}

	// No luck in given timeout
	return 1;
}

//*----------------------------------------------------------------------------
//* Function Name       : hw_dsp_eep_update_audio_gain
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void hw_dsp_eep_update_audio_gain(int value)
{
	struct DspTransceiverState 	temp_ts;	// just temp copy in stack
	ulong 						abs_addr;

	// Check for lock
	if(hw_dsp_eep_wait_lock())
		return;

	// Set value locally
	temp_ts.audio_gain = value;

	// Get absolute offset in structure
	abs_addr = (ulong)&(temp_ts.audio_gain) - (ulong)&(temp_ts.samp_rate);

	// Fill data in public request
	tsu.update_dsp_eep_offset 	= (ushort)abs_addr;
	tsu.update_dsp_eep_size 	= sizeof(temp_ts.audio_gain);
	tsu.update_dsp_eep_value 	= temp_ts.audio_gain;

	//printf("Update Audio Gain in DSP:\r\n");
	//printf("addr in struct:%d\r\n",	tsu.update_dsp_eep_offset );
	//printf("var size:%d\r\n",		tsu.update_dsp_eep_size);
	//printf("value:%d\r\n",			tsu.update_dsp_eep_value);

	// Post request
	tsu.update_dsp_eep_req = 1;
}

//*----------------------------------------------------------------------------
//* Function Name       : hw_dsp_eep_update_rf_gain
//* Object              : Needs AGC off + call of re-calc routine in ui driver(DSP code) ?
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void hw_dsp_eep_update_rf_gain(int value)
{
	struct DspTransceiverState 	temp_ts;	// just temp copy in stack
	ulong 						abs_addr;

	// Check for lock
	if(hw_dsp_eep_wait_lock())
		return;

	// Set value locally
	temp_ts.rf_gain = value;

	// Get absolute offset in structure
	abs_addr = (ulong)&(temp_ts.rf_gain) - (ulong)&(temp_ts.samp_rate);

	// Fill data in public request
	tsu.update_dsp_eep_offset 	= (ushort)abs_addr;
	tsu.update_dsp_eep_size 	= sizeof(temp_ts.rf_gain);
	tsu.update_dsp_eep_value 	= temp_ts.rf_gain;

	// Post request
	tsu.update_dsp_eep_req = 1;
}

//*----------------------------------------------------------------------------
//* Function Name       : hw_dsp_eep_update_rit
//* Object              : not tested!
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void hw_dsp_eep_update_rit(short value)
{
	struct DspTransceiverState 	temp_ts;	// just temp copy in stack
	ulong 						abs_addr;

	// Check for lock
	if(hw_dsp_eep_wait_lock())
		return;

	// Set value locally
	temp_ts.rit_value = value;

	// Get absolute offset in structure
	abs_addr = (ulong)&(temp_ts.rit_value) - (ulong)&(temp_ts.samp_rate);

	// Fill data in public request
	tsu.update_dsp_eep_offset 	= (ushort)abs_addr;
	tsu.update_dsp_eep_size 	= sizeof(temp_ts.rit_value);
	tsu.update_dsp_eep_value 	= temp_ts.rit_value;

	// Post request
	tsu.update_dsp_eep_req = 1;
}

//*----------------------------------------------------------------------------
//* Function Name       : hw_dsp_eep_set_agc_mode
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void hw_dsp_eep_set_agc_mode(uchar value)
{
	struct DspTransceiverState 	temp_ts;	// just temp copy in stack
	ulong 						abs_addr;

	// Check for lock
	if(hw_dsp_eep_wait_lock())
		return;

	// Set value locally
	temp_ts.agc_mode = value;

	// Get absolute offset in structure
	abs_addr = (ulong)&(temp_ts.agc_mode) - (ulong)&(temp_ts.samp_rate);

	// Fill data in public request
	tsu.update_dsp_eep_offset 	= (ushort)abs_addr;
	tsu.update_dsp_eep_size 	= sizeof(temp_ts.agc_mode);
	tsu.update_dsp_eep_value 	= temp_ts.agc_mode;

	// Post request
	tsu.update_dsp_eep_req = 1;
}

