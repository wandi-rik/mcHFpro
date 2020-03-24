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

#include "main.h"
#include "mchf_pro_board.h"

#ifdef CONTEXT_AUDIO

#include "audio_proc.h"

#include "stm32h747i_discovery_audio.h"
#include "audio.h"
#include "wm8994.h"

BSP_AUDIO_Init_t AudioPlayInit;

#define AUDIO_FILE_ADDRESS   0x08180000
#define AUDIO_FILE_SIZE      (180*1024)
#define PLAY_HEADER          0x2C
#define PLAY_BUFF_SIZE       4096
#define AUDIO_FREQUENCY_22K  22050U

//extern SAI_HandleTypeDef               haudio_out_sai;
extern SAI_HandleTypeDef               haudio_in_sai;

// Buffer location should aligned to cache line size (32 bytes)
__attribute__((section("heap_mem"))) ALIGN_32BYTES (uint16_t PlayBuff[PLAY_BUFF_SIZE]);

__IO int16_t        UpdatePointer = -1;
uint32_t 			PlaybackPosition   = PLAY_BUFF_SIZE + PLAY_HEADER;

void AUDIO_OUT_SAIx_DMAx_IRQHandler(void)
{
	BSP_AUDIO_OUT_IRQHandler(0);
}

/* SAI Audio IN DMA2_Stream4 */
void AUDIO_IN_SAIx_DMAx_IRQHandler(void)
{
	HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Instance)
{
	UpdatePointer = PLAY_BUFF_SIZE/2;
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Instance)
{
	UpdatePointer = 0;
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
	AudioPlayInit.Device 		= AUDIO_OUT_DEVICE_HEADPHONE;
	AudioPlayInit.ChannelsNbr 	= 2;
	AudioPlayInit.SampleRate 	= I2S_AUDIOFREQ_22K ;
	AudioPlayInit.BitsPerSample = AUDIO_RESOLUTION_16B;
	AudioPlayInit.Volume 		= 200;

	/* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */
	if(BSP_AUDIO_OUT_Init(0, &AudioPlayInit) != BSP_ERROR_NONE)
	{
		printf("err init 1\r\n");
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : audio_proc_worker
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_AUDIO
//*----------------------------------------------------------------------------
static void audio_proc_worker(void)
{
#if 1
	/* Wait a callback event */
    while(UpdatePointer==-1);

    int position = UpdatePointer;
    UpdatePointer = -1;

    /* Update the first or the second part of the buffer */
    for(int i = 0; i < PLAY_BUFF_SIZE/2; i++)
    {
      PlayBuff[i+position] = *(uint16_t *)(AUDIO_FILE_ADDRESS + PlaybackPosition);
      PlaybackPosition+=2;
    }

    /* Clean Data Cache to update the content of the SRAM */
    SCB_CleanDCache_by_Addr((uint32_t*)&PlayBuff[position], PLAY_BUFF_SIZE);

    /* check the end of the file */
    if((PlaybackPosition+PLAY_BUFF_SIZE/2) > AUDIO_FILE_SIZE)
    {
      PlaybackPosition = PLAY_HEADER;
    }

    if(UpdatePointer != -1)
    {
      /* Buffer update time is too long compare to the data transfer time */
    	printf("error update buffer!\r\n");
    }
	#endif
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
	// Delay start, so UI can paint properly
	vTaskDelay(1000);

	printf("audio proc start\r\n");

	// Check if the buffer has been loaded in flash
	if(*((uint64_t *)AUDIO_FILE_ADDRESS) != 0x017EFE2446464952 )
		printf("error file\r\n");
	
	// Initialize the data buffer
	for(int i=0; i < PLAY_BUFF_SIZE; i+=2)
	{
		PlayBuff[i]=*((__IO uint16_t *)(AUDIO_FILE_ADDRESS + PLAY_HEADER + i));
	}

	if(HAL_SAI_Transmit_DMA(&haudio_out_sai, (uint8_t *)PlayBuff, PLAY_BUFF_SIZE) != HAL_OK)
	{
		printf("error play %d\r\n", haudio_out_sai.ErrorCode);
	}
	printf("audio started\r\n");

audio_proc_loop:
	audio_proc_worker();
	goto audio_proc_loop;
}

#endif




