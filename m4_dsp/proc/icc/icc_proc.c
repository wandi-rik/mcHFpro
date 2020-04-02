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

#include "stm32h7xx_hal.h"
#include "stm32h747i_discovery.h"
#include "openamp.h"

//#ifdef CONTEXT_ICC

#include <stdio.h>

#include "icc_proc.h"
//#include "mchf_icc_def.h"

#include "audio_proc.h"
#include "audio_sai.h"		// temp to dump samples
#include "ui_driver.h"
#include "ui_rotary.h"

#define RPMSG_SERVICE_NAME              "stm32_icc_service"

static  uint32_t 						message;
static volatile int 					message_received;
static volatile unsigned int 			received_data;
static struct rpmsg_endpoint 			rp_endpoint;

HSEM_TypeDef * HSEM_DEBUG= HSEM;

// ------------------------------------------------
// Frequency public
extern __IO DialFrequency 		df;

// ------------------------------------------------
// Transceiver state public structure
extern __IO TransceiverState 	ts;

unsigned long ui_blink = 0;

unsigned char icc_out_buffer[RPMSG_BUFFER_SIZE - 32];
unsigned char icc_in_buffer [RPMSG_BUFFER_SIZE - 32];

void print_hex_array(uchar *pArray, ushort aSize)
{
	ulong i = 0,j = 0, c = 0;
	char *buf;

	buf = (char *)malloc(aSize * 10);
	memset(buf, 0, (aSize * 10));

	for (i = 0; i < aSize; i++)
	{
		j += sprintf( buf+j ,"%02x ", *pArray );
		pArray++;
		if (c++==15)
		{
			j += sprintf(buf+j ,"\r\n");
			c = 0;
		}
	}
	printf(buf);
	free(buf);
}

static int rpmsg_recv_callback(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
	received_data = *((unsigned int *) data);
	if(len < sizeof(icc_in_buffer))
	{
		uchar *payload = (uchar *)(data + 4);
		memcpy(icc_in_buffer, payload, (len - 4));
	}

	// Ack receive
	message_received = 1;

	return 0;
}

void icc_proc_post(void)
{
#ifndef API_DMA_MODE
	ulong k;
	ulong tune_loc;

	// Broadcast required for waterfall/scope data ?
	//if(as.api_broadcast_type != 1)
	//	return;

	//tune_loc = df.tune_new;
	//tune_loc = ts.tune_freq;

	// Clear buffer
	for(k = 0; k < 300;k++)
		icc_out_buffer[k] = 0;

	// ----------------------
	// Header
	icc_out_buffer[0x00] = 0x12; 						// signature
	icc_out_buffer[0x01] = 0x34;						// signature
	//icc_out_buffer[0x02] = as.led_s;					// blinker
	//icc_out_buffer[0x03] = as.pub_v;					// seq cnt

	// DSP Version
	//icc_out_buffer[0x04] = MCHF_D_VER_MAJOR;
	//icc_out_buffer[0x05] = MCHF_D_VER_MINOR;
	//icc_out_buffer[0x06] = MCHF_D_VER_RELEASE;
	//icc_out_buffer[0x07] = MCHF_D_VER_BUILD;

	// Frequency
	icc_out_buffer[0x08] = tune_loc >> 24;
	icc_out_buffer[0x09] = tune_loc >> 16;
	icc_out_buffer[0x0A] = tune_loc >>  8;
	icc_out_buffer[0x0B] = tune_loc >>  0;

	//icc_out_buffer[0x0C] = ts.dmod_mode;
	//icc_out_buffer[0x0D] = ts.band;
	//icc_out_buffer[0x0E] = ts.audio_gain;
	//icc_out_buffer[0x0F] = ts.filter_id;

	// Tuning step
	//icc_out_buffer[0x10] = df.selected_idx;

	// Local NCO freq
	//icc_out_buffer[0x11] = df.nco_freq >>  8;
	//icc_out_buffer[0x12] = df.nco_freq >>  0;

	// Return Logic board PCB revision
	//if(get_pcb_rev() == 0x08)
		icc_out_buffer[0x13] = 0x08;
	//else
	//	icc_out_buffer[0x13] = 0x07;

	#if 0
	// Insert FFT
	if(fft != NULL)
	{
		// Left part of screen
		for(k = 0; k < 128;k++)
			icc_out_buffer[k + 0x28] = (uchar)*(fft + k + 128);

		// Right part of screen
		for(k = 0; k < 128;k++)
			icc_out_buffer[k + 128 + 0x28] = (uchar)*(fft + k + 0);
	}
	#endif

	//audio_sai_get_buffer(icc_out_buffer + 0x28);
	ui_driver_get_buffer(icc_out_buffer + 0x28);

	// Footer
	icc_out_buffer[298] = 0x55;
	icc_out_buffer[299] = 0xAA;

	// Broadcast current state
	//api_dsp_to_cpu_msg(300);

	//(as.pub_v)++;
#endif
}

void icc_proc_fw_version(void)
{
	// DSP Version
	icc_out_buffer[0x00] = MCHF_D_VER_MAJOR;
	icc_out_buffer[0x01] = MCHF_D_VER_MINOR;
	icc_out_buffer[0x02] = MCHF_D_VER_RELEASE;
	icc_out_buffer[0x03] = MCHF_D_VER_BUILD;
}

//*----------------------------------------------------------------------------
//* Function Name       : icc_proc_hw_init
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ICC
//*----------------------------------------------------------------------------
void icc_proc_hw_init(void)
{
	int32_t status = 0;

	printf("icc init...\r\n");

	// Init the mailbox use notify the other core on new message
	MAILBOX_Init();

	// Init OpenAmp and libmetal libraries
	if (MX_OPENAMP_Init(RPMSG_REMOTE, NULL) != HAL_OK)
	{
		printf("err 1\r\n");
		return;
	}

	// create a endpoint for rmpsg communication
	status = OPENAMP_create_endpoint(&rp_endpoint, RPMSG_SERVICE_NAME, RPMSG_ADDR_ANY, rpmsg_recv_callback, NULL);
	if (status < 0)
	{
		printf("err 2\r\n");
		return;
	}

	// Init values
	ts.api_band 		= 0;
	df.tune_upd 		= 0;
	ts.api_iamb_type 	= 0;

	ts.dmod_mode 		= DEMOD_LSB;
	ts.audio_gain 		= 6;
	ts.filter_id 		= 3;
	df.nco_freq			= -6000;
}

//*----------------------------------------------------------------------------
//* Function Name       : icc_proc_cmd_handler
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ICC
//*----------------------------------------------------------------------------
static uchar icc_proc_cmd_handler(uchar cmd)
{
	switch(cmd)
	{
		// Waterfall/spectrum FFT data to M7 core
		case ICC_BROADCAST:
			icc_proc_post();
			break;

		// Start SAI driver
		case ICC_START_I2S_PROC:
			audio_driver_init();
			break;

		// Blinking LED
		case ICC_TOGGLE_LED:
			BSP_LED_Toggle(LED_BLUE);
			break;

		// Return FW version
		case ICC_GET_FW_VERSION:
			icc_proc_fw_version();
			break;

		// Update local transceiver state
		case ICC_SET_TRX_STATE:
		{
			//printf("msg 4, payload %d\r\n",icc_in_buffer[6]);

			print_hex_array(icc_in_buffer, 16);
			break;
		}

		default:
			printf("unknown msg %d\r\n",cmd);
			return 0;
	}

	return cmd;
}

//*----------------------------------------------------------------------------
//* Function Name       : icc_proc_worker
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ICC
//*----------------------------------------------------------------------------
static void icc_proc_worker(void)
{
	int32_t status = 0;

	OPENAMP_check_for_message();

	// Something to process ?
	if(message_received == 0)
	  return;

	// Clear flag
	message_received = 0;

	// Process command
	message = icc_proc_cmd_handler((uchar)received_data);

	// Response to M7 core
	status = OPENAMP_send(&rp_endpoint, icc_out_buffer, 300);
	if (status < 0)
	{
		printf("err send msg: %d (cmd: %d)\r\n", status, message);
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : icc_proc_task
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ICC
//*----------------------------------------------------------------------------
void icc_proc_task(void const * argument)
{
	icc_proc_worker();
}
