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

#include "hw\openamp.h"
#include "stm32h747i_discovery_audio.h"

#include "icc_proc.h"
//#include "mchf_icc_def.h"

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

extern struct TransceiverState 	ts;

#ifdef CONTEXT_DRIVER_API

static void api_ui_process_broadcast(void)
{
	ulong i,temp;

	#ifndef API_DRIVER_USE_DMA
	// Data waiting ?
	if(!rx_done) return;
	#endif

	#ifdef API_DRIVER_USE_DMA
	//
	//#ifndef H7_DISABLE_DATA_CACHE
	//SCB_InvalidateDCache_by_Addr((uint32_t *)(ucDmaRxBuffer),API_DMA_RX_B_SIZE);
	//#endif
	//
	//for(i = 0; i < API_DMA_RX_B_SIZE; i++)
	//	aRxBuffer[i] = ucDmaRxBuffer[i];
	#endif

	// Data valid ?
	if((aRxBuffer[0] != 0x12) || (aRxBuffer[1] != 0x34) || (aRxBuffer[298] != 0x55) || (aRxBuffer[299] != 0xAA))
	{
		//printf("sig: %02x %02x\r\n",aRxBuffer[0],aRxBuffer[1]);
		goto exit_anyway;
	}

	// Check seq no
	if(tsu.dsp_seq_number_old == aRxBuffer[3])
		tsu.dsp_alive = 0;
	else
		tsu.dsp_alive = 1;

	// Save sequence number for next transfer
	tsu.dsp_seq_number_old = aRxBuffer[3];

	// Get blinker
	tsu.dsp_blinker = aRxBuffer[2];

	// Get firmware version
	tsu.dsp_rev1 = aRxBuffer[4];
	tsu.dsp_rev2 = aRxBuffer[5];
	tsu.dsp_rev3 = aRxBuffer[6];
	tsu.dsp_rev4 = aRxBuffer[7];

	// Load DSP frequency
	temp  = aRxBuffer[0x08] << 24;
	temp |= aRxBuffer[0x09] << 16;
	temp |= aRxBuffer[0x0A] <<  8;
	temp |= aRxBuffer[0x0B] <<  0;
	tsu.dsp_freq = temp;	//temp/4;
	//printf("dsp freq = %d\r\n",tsu.dsp_freq);

	// Has local freq being invalidated by band change ?
	//if(tsu.vfo_a == 0xFFFFFFFF)
	//	tsu.vfo_a = tsu.dsp_freq;

	// Temp, load initial VFO B here
	//if(tsu.vfo_b == 0xFFFFFFFF)
	//	tsu.vfo_b = tsu.dsp_freq;

	// Get demod mode
	tsu.dsp_demod  = aRxBuffer[0x0C];

	// Has local demodulator mode being invalidated by band change ?
	//if(tsu.demod_mode == 0xFF)
	//	tsu.demod_mode = tsu.dsp_demod;

	// Get band
	tsu.dsp_band   = aRxBuffer[0x0D];
	//printf("dsp band = %d\r\n",tsu.dsp_band);

	// Volume level in DSP
	tsu.dsp_volume = aRxBuffer[0x0E];

	// Filter selection in DSP
	tsu.dsp_filter = aRxBuffer[0x0F];
	//printf("tsu.dsp_filter = %d\r\n",tsu.dsp_filter);

	// Has local filter being invalidated by band change ?
	//if(tsu.curr_filter == 0xFF)
	//	tsu.curr_filter = tsu.dsp_filter;

	// Tuning step in DSP, but as ptr id, not actual value
	tsu.dsp_step_idx = aRxBuffer[0x10];

	// NCO freq in DSP
	tsu.dsp_nco_freq  = aRxBuffer[0x11] <<  8;
	tsu.dsp_nco_freq |= aRxBuffer[0x12] <<  0;
	//printf("dsp_nco_freq %d\r\n",tsu.dsp_nco_freq);

	tsu.pcb_rev = aRxBuffer[0x13];

	// Copy FFT
	for(i = 0; i < 256; i++)
		ui_sw.fft_dsp[i] = aRxBuffer[i + 0x28];

exit_anyway:

	// Acknowledge next
	rx_done = 0;
}

static void api_ui_send_fast_cmd(void)
{
	ulong i,temp;

	for(i = 0; i < 16; i++)
		aTxBuffer[i] = 0;

	#ifndef API_DRIVER_USE_DMA
	if(rx_active)
		return;
	#endif

#if 0
	// Change band - always first as highest priority call
	if(tsu.update_band_dsp_req)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("tsu.curr_band = %d\r\n",tsu.curr_band);
		#endif

		// cmd
		aTxBuffer[0x00] = (API_UPD_BAND >>   8);
		aTxBuffer[0x01] = (API_UPD_BAND & 0xFF);
		// data
		aTxBuffer[0x02] = tsu.curr_band;

		// Send
		api_ui_send_spi();
		// Clear flag
		tsu.update_band_dsp_req = 0;
		//
		return;
	}
#endif
#if 1
	// Update frequency
	if(tsu.update_freq_dsp_req)
	{
		if(tsu.band[tsu.curr_band].active_vfo == 0)
			temp = (tsu.band[tsu.curr_band].vfo_a);
		else
			temp = (tsu.band[tsu.curr_band].vfo_b);

		#ifdef API_UI_ALLOW_DEBUG
		printf("update DSP freq = %d\r\n",temp);
		#endif

		// Do in DSP ??
		temp *= 4;

		// cmd
		aTxBuffer[0x00] = (API_UPD_FREQ >>   8);
		aTxBuffer[0x01] = (API_UPD_FREQ & 0xFF);
		// data
		aTxBuffer[0x02] = temp >> 24;
		aTxBuffer[0x03] = temp >> 16;
		aTxBuffer[0x04] = temp >>  8;
		aTxBuffer[0x05] = temp >>  0;
		// Send
		api_ui_send_spi();
		// Reset flag
		tsu.update_freq_dsp_req  = 0;
		//
		return;
	}
#endif
#if 1
	// NCO frequency
	if(tsu.update_nco_dsp_req)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("nco freq = %d\r\n",tsu.band[tsu.curr_band].nco_freq);
		#endif

		aTxBuffer[0x00] = (API_UPD_NCO >>   8);
		aTxBuffer[0x01] = (API_UPD_NCO & 0xFF);
		//
		aTxBuffer[0x02] = tsu.band[tsu.curr_band].nco_freq >>  8;
		aTxBuffer[0x03] = tsu.band[tsu.curr_band].nco_freq >>  0;
		// Send
		api_ui_send_spi();
		// Reset flag
		tsu.update_nco_dsp_req = 0;
		return;
	}
#endif
#if 1
	// Update audio volume
	if(tsu.update_audio_dsp_req)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("tsu.audio_volume = %d\r\n",tsu.band[tsu.curr_band].volume);
		#endif

		aTxBuffer[0x00] = (API_UPD_VOL >>   8);
		aTxBuffer[0x01] = (API_UPD_VOL & 0xFF);
		//
		aTxBuffer[0x02] = tsu.band[tsu.curr_band].volume;
		// Send
		api_ui_send_spi();
		// Reset flag
		tsu.update_audio_dsp_req = 0;
		//
		return;
	}
#endif
#if 1
	// Update demod mode
	//if((tsu.dsp_demod != tsu.demod_mode) && (tsu.demod_mode != 0xFF))
	if(tsu.update_demod_dsp_req)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("demod_mode = %d\r\n",tsu.band[tsu.curr_band].demod_mode);
		#endif

		aTxBuffer[0x00] = (API_UPD_DEMOD >>   8);
		aTxBuffer[0x01] = (API_UPD_DEMOD & 0xFF);
		//
		aTxBuffer[0x02] = tsu.band[tsu.curr_band].demod_mode;
		// Send
		api_ui_send_spi();
		// Reset flag
		tsu.update_demod_dsp_req = 0;
		return;
	}
#endif
#if 1
	// Change filter
	//if((tsu.dsp_filter != tsu.curr_filter) && (tsu.curr_filter != 0xFF))
	if(tsu.update_filter_dsp_req)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("filter = %d\r\n",tsu.band[tsu.curr_band].filter);
		#endif

		aTxBuffer[0x00] = (API_UPD_FILT >>   8);
		aTxBuffer[0x01] = (API_UPD_FILT & 0xFF);
		//
		aTxBuffer[0x02] = tsu.band[tsu.curr_band].filter;
		// Send
		api_ui_send_spi();
		//  Reset flag
		tsu.update_filter_dsp_req = 0;
		return;
	}
#endif
#if 0
	// Tuning step
	//if((tsu.step_idx != tsu.dsp_step_idx) && (tsu.step_idx != 0xFF))
	//{
		//#ifdef API_UI_ALLOW_DEBUG
		//printf("tsu.step_idx = %d\r\n",tsu.step_idx);
		//#endif

		aTxBuffer[0x00] = (API_UPD_STEP >>   8);
		aTxBuffer[0x01] = (API_UPD_STEP & 0xFF);
		//
		aTxBuffer[0x02] = 3;//tsu.step_idx;

		api_ui_send_spi();
		return;
	//}
#endif
#if 1
	// CW TX test
	if(tsu.cw_tx_state)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("cw_tx_state = %d\r\n",tsu.cw_tx_state);
		//printf("tsu.cw_iamb_type = %d\r\n",tsu.cw_iamb_type);
		#endif

		aTxBuffer[0x00] = (API_CW_TX >>   8);
		aTxBuffer[0x01] = (API_CW_TX & 0xFF);
		//
		if(tsu.cw_tx_state == 1)
			aTxBuffer[0x02] = 1;					// tx on

		if(tsu.cw_tx_state == 2)
			aTxBuffer[0x02] = 0;					// tx off

		aTxBuffer[0x03] = tsu.cw_iamb_type;			// iambic line

		api_ui_send_spi();

		// Release state no longer needed
		if(tsu.cw_tx_state == 2)
		{
			tsu.cw_tx_state = 0;

			#ifdef API_UI_ALLOW_DEBUG
			printf("reset ok\r\n");
			#endif
		}

		return;
	}
#endif
#if 1
	// Write into DSP eeprom structure
	if(tsu.update_dsp_eep_req)
	{
		ushort offset = 0, size = 0;

		#ifdef API_UI_ALLOW_DEBUG
		printf("update DSP eep request process..\r\n");
		#endif

		aTxBuffer[0x00] = (API_WRITE_EEP >>   8);
		aTxBuffer[0x01] = (API_WRITE_EEP & 0xFF);
		// Offset
		aTxBuffer[0x02] = tsu.update_dsp_eep_offset >> 8;
		aTxBuffer[0x03] = tsu.update_dsp_eep_offset;
		// size
		aTxBuffer[0x04] = tsu.update_dsp_eep_size;
		// value
		aTxBuffer[0x05] = tsu.update_dsp_eep_value >> 24;
		aTxBuffer[0x06] = tsu.update_dsp_eep_value >> 16;
		aTxBuffer[0x07] = tsu.update_dsp_eep_value >>  8;
		aTxBuffer[0x08] = tsu.update_dsp_eep_value;
		// Send
		api_ui_send_spi();
		//  Reset flag
		tsu.update_dsp_eep_req = 0;
		return;
	}
#endif
#if 0								// moved to new function
	// Send DSP restart request
	if(tsu.update_dsp_restart)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("restart DSP request process..\r\n");
		#endif

		aTxBuffer[0x00] = (API_RESTART >>   8);
		aTxBuffer[0x01] = (API_RESTART & 0xFF);
		// Send
		api_ui_send_spi();
		//  Reset flag
		tsu.update_dsp_restart = 0;
		return;
	}
#endif
}

// This version using messaging, instead of public flags
//
// - work in progress
//
static void api_ui_send_fast_cmd_a(void)
{
	osEvent event;

	// Wait for a short time for pending messages
	event = osMessageGet(hApiMessage, 20);
	if(event.status != osEventMessage)
		return;												// ideally this should fire every 20mS

	// Check status type
	if(event.status != osEventMessage)
		return;												// is that even possible ?

	// Send
	api_ui_send_spi_a(event.value.p);
}
#endif


#ifdef CONTEXT_ICC

#define RPMSG_CHAN_NAME              "stm32_icc_service"

static  uint32_t	 			message = 0;
static volatile int	 			message_received = 0;
static volatile int 			service_created = 0;
//static volatile unsigned int 	received_data;
static struct rpmsg_endpoint 	rp_endpoint;

uchar 							dsp_remote_init_done = 0;
uchar 							dsp_delayed_init_state = 0;

uchar 							aRxBuffer[RPMSG_BUFFER_SIZE - 32];		// ToDo: try malloc per cmd ???
uchar 							aTxBuffer[RPMSG_BUFFER_SIZE - 32];		//		 --------------------

// Spectrum control publics
extern struct 		UI_SW		ui_sw;

static int rpmsg_recv_callback(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
	//printf("rx size: %d\r\n",len);

	if(len <= sizeof(aRxBuffer))
		memcpy(aRxBuffer,data,len);

	//received_data = *((unsigned int *) data);
	message_received=1;

	return 0;
}

void service_destroy_cb(struct rpmsg_endpoint *ept)
{
  /* this function is called while remote endpoint as been destroyed, the
   * service is no more available
   */
}

void new_service_cb(struct rpmsg_device *rdev, const char *name, uint32_t dest)
{
	// create a endpoint for rmpsg communication
	OPENAMP_create_endpoint(&rp_endpoint, name, dest, rpmsg_recv_callback, service_destroy_cb);

	service_created = 1;

	printf("== created ==\r\n");
}

static void api_ui_process_broadcast(void)
{
	ulong i,temp;

	// Test only ------------------
	static uchar skip_bc = 0;
	skip_bc++;
	if(skip_bc < 2) return;
	skip_bc = 0;
	// ----------------------------

	// Data valid ?
//	if((aRxBuffer[0] != 0x12) || (aRxBuffer[1] != 0x34) || (aRxBuffer[298] != 0x55) || (aRxBuffer[299] != 0xAA))
//	{
//		printf("sig: %02x %02x\r\n",aRxBuffer[0],aRxBuffer[1]);
//		goto exit_anyway;
//	}

	// Check seq no
	//if(tsu.dsp_seq_number_old == aRxBuffer[3])
	//	tsu.dsp_alive = 0;
	//else
	//	tsu.dsp_alive = 1;

	// Save sequence number for next transfer
	//tsu.dsp_seq_number_old = aRxBuffer[3];

	// Get blinker
	//tsu.dsp_blinker = aRxBuffer[2];

	// Get firmware version
	//tsu.dsp_rev1 = aRxBuffer[4];
	//tsu.dsp_rev2 = aRxBuffer[5];
	//tsu.dsp_rev3 = aRxBuffer[6];
	//tsu.dsp_rev4 = aRxBuffer[7];

	// Load DSP frequency
//	temp  = aRxBuffer[0x08] << 24;
//	temp |= aRxBuffer[0x09] << 16;
//	temp |= aRxBuffer[0x0A] <<  8;
//	temp |= aRxBuffer[0x0B] <<  0;
//	tsu.dsp_freq = temp;	//temp/4;
	//printf("dsp freq = %d\r\n",tsu.dsp_freq);

	// Has local freq being invalidated by band change ?
	//if(tsu.vfo_a == 0xFFFFFFFF)
	//	tsu.vfo_a = tsu.dsp_freq;

	// Temp, load initial VFO B here
	//if(tsu.vfo_b == 0xFFFFFFFF)
	//	tsu.vfo_b = tsu.dsp_freq;

	// Get demod mode
//	tsu.dsp_demod  = aRxBuffer[0x0C];

	// Has local demodulator mode being invalidated by band change ?
	//if(tsu.demod_mode == 0xFF)
	//	tsu.demod_mode = tsu.dsp_demod;

	// Get band
//	tsu.dsp_band   = aRxBuffer[0x0D];
	//printf("dsp band = %d\r\n",tsu.dsp_band);

	// Volume level in DSP
//	tsu.dsp_volume = aRxBuffer[0x0E];

	// Filter selection in DSP
//	tsu.dsp_filter = aRxBuffer[0x0F];
	//printf("tsu.dsp_filter = %d\r\n",tsu.dsp_filter);

	// Has local filter being invalidated by band change ?
	//if(tsu.curr_filter == 0xFF)
	//	tsu.curr_filter = tsu.dsp_filter;

	// Tuning step in DSP, but as ptr id, not actual value
//	tsu.dsp_step_idx = aRxBuffer[0x10];

	// NCO freq in DSP
//	tsu.dsp_nco_freq  = aRxBuffer[0x11] <<  8;
//	tsu.dsp_nco_freq |= aRxBuffer[0x12] <<  0;
	//printf("dsp_nco_freq %d\r\n",tsu.dsp_nco_freq);

//	tsu.pcb_rev = aRxBuffer[0x13];

	// Copy FFT
	for(i = 0; i < 1024; i++)
		ui_sw.fft_dsp[i] = aRxBuffer[i];

	//printf("got fft\r\n");

exit_anyway:
	return;
	// Acknowledge next
	//rx_done = 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : icc_proc_wake_second_core
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ICC
//*----------------------------------------------------------------------------
static ulong icc_proc_wake_second_core(void)
{
	int32_t timeout = 0xFFFF;

	HAL_HSEM_FastTake(HSEM_ID_0);

	// Release HSEM in order to notify the CPU2(CM4)
	HAL_HSEM_Release(HSEM_ID_0, 0);

	// wait until CPU2 wakes up from stop mode
	while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
	if(timeout < 0)
		return 1;

	return 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : icc_proc_cmd_xchange
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ICC
//*----------------------------------------------------------------------------
uchar icc_proc_cmd_xchange(uchar cmd, uchar *buff, ushort size)
{
	int32_t status;

	// Payload or just command
	if((buff != NULL) && (size != 0) && (size < (sizeof(aRxBuffer) - 4)))
	{
		aRxBuffer[0] = cmd;
		memcpy(aRxBuffer + 4, buff, size);
		status = OPENAMP_send(&rp_endpoint, aRxBuffer, (size + 4));
	}
	else
	{
		message = cmd;
		status = OPENAMP_send(&rp_endpoint, &message, sizeof(message));
	}

	if(status < 0)
	{
		printf("unable to send msg to core!\r\n");
		return 1;
	}

	// ToDo: timeout here...
	//
	while (message_received == 0)
	{
		osDelay(1);
		OPENAMP_check_for_message();
	}
	message_received = 0;

	//printf("data: %02x\r\n", received_data);
	return 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : icc_proc_init_hw
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ICC
//*----------------------------------------------------------------------------
uchar icc_proc_init_hw(void)
{
	// Initialise the mailbox use notify the other core on new message
	MAILBOX_Init();

	// Initialise the rpmsg endpoint to set default addresses to RPMSG_ADDR_ANY
	rpmsg_init_ept(&rp_endpoint, RPMSG_CHAN_NAME, RPMSG_ADDR_ANY, RPMSG_ADDR_ANY, NULL, NULL);

	// Initialise OpenAmp and libmetal libraries
	//
	// Note: in order to not fail here, we need to point openAmp malloc() and free() impl to
	//	     FreeRTOS allocator. There must be a way(undocumented) but easiest is to
	//		 modify "libmetal\lib\include\metal\system\generic\alloc.h"
	//
	if (MX_OPENAMP_Init(RPMSG_MASTER, new_service_cb) != HAL_OK)
	{
		printf("unable to init OpenAMP libs!\r\n");
		return 1;
	}
	printf("will wait remote processor...\r\n");

	// The rpmsg service is initiate by the remote processor, on A7 new_service_cb
	// callback is received on service creation. Wait for the callback
	if(OPENAMP_Wait_EndPointready(&rp_endpoint) != 0)
	{
		printf( "====================\r\n");
		printf( "====  DSP FAULT ====\r\n");
		printf( "====================\r\n");

		// Yeah, it is a critical error ;(
		NVIC_SystemReset();
	}

	// Callback activated ?
	if(service_created == 0)
	{
		printf("error service creation!\r\n");
		return 2;
	}

	//printf("send first message...\r\n");

	// Start I2S process - dummy call to set comms
	if(icc_proc_cmd_xchange(ICC_START_ICC_INIT, NULL, 0))
	{
		printf("error first cmd!\r\n");
		return 3;
	}
	printf("m4 core init done.\r\n");

	return 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : icc_proc_blink_remote_led
//* Object              :
//* Notes    			: Every 300 mS
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ICC
//*----------------------------------------------------------------------------
void icc_proc_blink_remote_led(void)
{
	static ulong ul_blink = 0;

	ul_blink++;
	if(ul_blink < 30)
		return;

	// EVAL only ???
	if(icc_proc_cmd_xchange(ICC_TOGGLE_LED, NULL, 0) != 0)
	{
		tsu.dsp_alive = 0;								// Ack to UI lost comm to DSP
		printf("dsp not alive!\r\n");
	}
	else
		tsu.dsp_blinker = !tsu.dsp_blinker;				// Fill blinker

	ul_blink = 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : icc_proc_delayed_dsp_init
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ICC
//*----------------------------------------------------------------------------
void icc_proc_delayed_dsp_init(void)
{
	// Update state machine only if DSP is still alive and init not done
	if((tsu.dsp_alive == 0) || (dsp_remote_init_done == 1))
		return;

	switch(dsp_delayed_init_state)
	{
		// Read running DSP firmware version
		case 0:
		{
			if(icc_proc_cmd_xchange(ICC_GET_FW_VERSION, NULL, 0) == 0)
			{
				tsu.dsp_rev1 = aRxBuffer[0];
				tsu.dsp_rev2 = aRxBuffer[1];
				tsu.dsp_rev3 = aRxBuffer[2];
				tsu.dsp_rev4 = aRxBuffer[3];
				//printf("DSP:%d.%d.%d.%d\r\n",tsu.dsp_rev1,tsu.dsp_rev2,tsu.dsp_rev3,tsu.dsp_rev4);
			}
			else
				tsu.dsp_alive = 0;

			break;
		}

		// Upload transceiver state structure local copy
		case 1:
		{
			uchar  *out_ptr = (uchar *)(&ts.samp_rate);	// struct base ptr from address of it's first element!
			ushort out_size = sizeof(ts);

			#if 0
			printf("to send:%d\r\n", out_size);

			ulong i, x = 0;

			for(i = 0; i < 432; i++)
				x += *(out_ptr + i);

			printf("chksum %d\r\n", x);
			#endif

			icc_proc_cmd_xchange(ICC_SET_TRX_STATE, out_ptr, out_size);
			break;
		}

		case 2:
		{
			icc_proc_cmd_xchange(ICC_START_I2S_PROC, NULL, 0);
			break;
		}

		default:
			dsp_remote_init_done = 1;
			return;
	}

	// Next state
	dsp_delayed_init_state++;
}

//*----------------------------------------------------------------------------
//* Function Name       : icc_proc_task
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ICC
//*----------------------------------------------------------------------------
void icc_proc_task(void const *arg)
{
	int32_t 	status;
	uint32_t 	ulNotificationValue = 0, ulNotif;

	vTaskDelay(3000);

	printf("icc driver start\r\n");

	// Wake up M4 core
	if(icc_proc_wake_second_core())
	{
		printf("unable to wake-up M4 core!\r\n");
		goto icc_init_failed;
	}

	// HSEM is bit out of sync in the beginning
	vTaskDelay(200);

	// Init ICC comms
	if(icc_proc_init_hw())
		goto icc_init_failed;

	// Second core is up and running
	tsu.dsp_alive = 1;

icc_proc_loop:

	// Update state machine
	icc_proc_delayed_dsp_init();

	// Just run the delayed init state machine
	if(dsp_remote_init_done == 0)
	{
		vTaskDelay(20);
		goto icc_proc_loop;
	}

	if((ulNotif) && (ulNotificationValue))
	{
		printf("command to route to M4: %02x\r\n", ulNotificationValue);

		// Send command to M4 from any task
		if(icc_proc_cmd_xchange(ulNotificationValue & 0xFF, NULL, 0) != 0)
			tsu.dsp_alive = 0;
	}
	else
	{
		// Just a gimmick really
		icc_proc_blink_remote_led();

		// Initiate broadcast - 100Hz FFT buffer refresh
		if(icc_proc_cmd_xchange(ICC_BROADCAST, NULL, 0) == 0)
			api_ui_process_broadcast();
		else
			tsu.dsp_alive = 0;
	}

	// Sleep and wait notification
	ulNotif = xTaskNotifyWait(0x00, ULONG_MAX, &ulNotificationValue, 40);

	// Loop execution
	goto icc_proc_loop;

	// Disable driver
icc_init_failed:
	vTaskSuspend(NULL);
}

#endif
