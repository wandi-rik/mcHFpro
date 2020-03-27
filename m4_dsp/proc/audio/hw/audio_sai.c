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

#include <stdio.h>
#include <string.h>

#include "audio_sai.h"

#include "stm32h7xx_hal.h"
#include "stm32h747i_discovery.h"
#include "stm32h747i_discovery_audio.h"
#include "audio.h"
#include "wm8994.h"

#include "audio_proc.h"

BSP_AUDIO_Init_t  				AudioInInit;
BSP_AUDIO_Init_t 				AudioOutInit;

extern SAI_HandleTypeDef		haudio_out_sai;
extern SAI_HandleTypeDef		haudio_in_sai;

ALIGN_32BYTES (uint16_t TxBuff[DMA_BUFF_SIZE]);
ALIGN_32BYTES (uint16_t RxBuff[DMA_BUFF_SIZE]);

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
	//
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Instance)
{
	//
}

void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
	I2S_RX_CallBack(RxBuff + DMA_BUFF_SIZE/2, TxBuff + DMA_BUFF_SIZE/2, DMA_BUFF_SIZE/2, 0);
}

void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
	I2S_RX_CallBack(RxBuff + 0, TxBuff + 0, DMA_BUFF_SIZE/2, 1);
}

//*----------------------------------------------------------------------------
//* Function Name       : audio_sai_hw_init
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_AUDIO
//*----------------------------------------------------------------------------
void audio_sai_hw_init(void)
{
	printf("sai init...\r\n");

	AudioOutInit.Volume 		= 80;
	AudioInInit.Volume 			= 40;

	if(BSP_AUDIO_INOUT_Init(0, &AudioOutInit, &AudioInInit) != BSP_ERROR_NONE)
	{
		printf("err tx init\r\n");
		return;
	}

	if(HAL_SAI_Receive_DMA(&haudio_in_sai, (uint8_t *)RxBuff, DMA_BUFF_SIZE) != HAL_OK)
	{
		printf("error rx dma %d\r\n", haudio_in_sai.ErrorCode);
		return;
	}

	if(HAL_SAI_Transmit_DMA(&haudio_out_sai, (uint8_t *)TxBuff, DMA_BUFF_SIZE) != HAL_OK)
	{
		printf("error tx dma %d\r\n", haudio_out_sai.ErrorCode);
		return;
	}

	printf("sai started\r\n");
}

//*----------------------------------------------------------------------------
//* Function Name       : audio_sai_get_buffer
//* Object              :
//* Notes    			: temp here!!!!
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_AUDIO
//*----------------------------------------------------------------------------
void audio_sai_get_buffer(uchar *buffer)
{
	memcpy(buffer, RxBuff, DMA_BUFF_SIZE);
}


