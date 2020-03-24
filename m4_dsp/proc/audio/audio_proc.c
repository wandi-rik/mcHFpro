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

//#ifdef CONTEXT_AUDIO

#include <stdio.h>
#include <string.h>

#include "audio_proc.h"

#include "stm32h7xx_hal.h"
#include "stm32h747i_discovery.h"
#include "stm32h747i_discovery_audio.h"
#include "audio.h"
#include "wm8994.h"

#include "audio_driver.h"

BSP_AUDIO_Init_t  			AudioInInit;
BSP_AUDIO_Init_t 			AudioOutInit;

#define AUDIO_FILE_ADDRESS   0x08180000
#define AUDIO_FILE_SIZE      (180*1024)
#define PLAY_HEADER          0x2C
#define PLAY_BUFF_SIZE       4096
#define AUDIO_FREQUENCY_22K  22050U

extern SAI_HandleTypeDef               haudio_out_sai;
extern SAI_HandleTypeDef               haudio_in_sai;

ALIGN_32BYTES (uint16_t TxBuff[PLAY_BUFF_SIZE]);
ALIGN_32BYTES (uint16_t RxBuff[PLAY_BUFF_SIZE]);

uint32_t 			PlaybackPosition   = PLAY_BUFF_SIZE + PLAY_HEADER;

ulong tx_ptr = 0;

uchar audio_init_done = 0;

void AUDIO_OUT_SAIx_DMAx_IRQHandler(void)
{
	HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

void AUDIO_IN_SAIx_DMAx_IRQHandler(void)
{
	HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Instance)
{
	#if 1
	// Fill from middle
	int position = PLAY_BUFF_SIZE/2;

	for(int i = 0; i < PLAY_BUFF_SIZE/2; i++)
	{
		TxBuff[i+position] = *(uint16_t *)(AUDIO_FILE_ADDRESS + PlaybackPosition);
		PlaybackPosition+=2;
	}

	// check the end of the file
	if((PlaybackPosition+PLAY_BUFF_SIZE/2) > AUDIO_FILE_SIZE)
		PlaybackPosition = PLAY_HEADER;
	#endif
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Instance)
{
	#if 1
	// Fill from start
	int position = 0;

    for(int i = 0; i < PLAY_BUFF_SIZE/2; i++)
    {
    	TxBuff[i+position] = *(uint16_t *)(AUDIO_FILE_ADDRESS + PlaybackPosition);
    	PlaybackPosition+=2;
    }

    // check the end of the file
    if((PlaybackPosition+PLAY_BUFF_SIZE/2) > AUDIO_FILE_SIZE)
    	PlaybackPosition = PLAY_HEADER;
	#endif
}

void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
}

void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
}

//*----------------------------------------------------------------------------
//* Function Name       : audio_proc_hw_init
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_AUDIO
//*----------------------------------------------------------------------------
void audio_proc_hw_init(void)
{
	printf("audio init...\r\n");

	if(audio_init_done)
		return;

	//audio_driver_init();

	AudioOutInit.Device 		= AUDIO_OUT_DEVICE_HEADPHONE;
	AudioOutInit.ChannelsNbr 	= 2;
	AudioOutInit.SampleRate 	= I2S_AUDIOFREQ_22K ;
	AudioOutInit.BitsPerSample 	= AUDIO_RESOLUTION_16B;
	AudioOutInit.Volume 		= 80;

	AudioInInit.Device 			= AUDIO_IN_DEVICE_ANALOG_MIC;
	AudioInInit.ChannelsNbr 	= 2;
	AudioInInit.SampleRate 		= I2S_AUDIOFREQ_22K ;
	AudioInInit.BitsPerSample 	= AUDIO_RESOLUTION_16B;
	AudioInInit.Volume 			= 40;

	if(BSP_AUDIO_INOUT_Init(0, &AudioOutInit, &AudioInInit) != BSP_ERROR_NONE)
	{
		printf("err tx init\r\n");
		return;
	}

	#if 1
	// Check if the buffer has been loaded in flash
	if(*((uint64_t *)AUDIO_FILE_ADDRESS) != 0x017EFE2446464952 )
	{
		printf("error file\r\n");
		return;
	}

	// Initialise the data buffer
	for(int i=0; i < PLAY_BUFF_SIZE; i+=2)
	{
		TxBuff[i]=*((__IO uint16_t *)(AUDIO_FILE_ADDRESS + PLAY_HEADER + i));
	}
	#endif

	#if 1
	if(HAL_SAI_Receive_DMA(&haudio_in_sai, (uint8_t *)RxBuff, PLAY_BUFF_SIZE) != HAL_OK)
	{
		printf("error play %d\r\n", haudio_in_sai.ErrorCode);
		return;
	}

	if(HAL_SAI_Transmit_DMA(&haudio_out_sai, (uint8_t *)RxBuff, PLAY_BUFF_SIZE) != HAL_OK)
	{
		printf("error play %d\r\n", haudio_out_sai.ErrorCode);
		return;
	}
	#endif

	printf("audio started\r\n");
	audio_init_done = 1;
}

void audio_proc_get_buffer(uchar *buffer)
{
	memcpy(buffer, TxBuff, 256);
}

//*----------------------------------------------------------------------------
//* Function Name       : audio_proc_task
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_AUDIO
//*----------------------------------------------------------------------------
void audio_proc_task(void const * argument)
{

}

//#endif




