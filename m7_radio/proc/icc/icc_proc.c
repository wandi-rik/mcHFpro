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

#include "icc_proc.h"
#include "mchf_icc_def.h"

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

#ifdef CONTEXT_DRIVER_API

#include "ui_driver.h"
#include "icc_proc.h"

#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_cortex.h"
#include "stm32h7xx_hal_spi.h"
#include "stm32h7xx_hal_dma.h"

#ifdef API_DRIVER_USE_DMA
//
enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};
//
__IO uint32_t wTransferState = TRANSFER_COMPLETE;
//
DMA_HandleTypeDef hdma_tx;
DMA_HandleTypeDef hdma_rx;
//
// DMA RX buffer in SRAM3 (also might be in use by the WSPR decoder)
// - buffers 32 bytes aligned!
uchar 				*ucDmaTxBuffer = (uchar *)0x30040000;
uchar 				*ucDmaRxBuffer = (uchar *)0x30040140;
//
#endif

// DMA buffers
uchar 				DmaInBuffer[2048];	//API_TRANSFER_SIZE
uchar 				DmaOuBuffer[API_TRANSFER_SIZE];
// Local buffers
uchar 				aRxBuffer[2048];	//API_TRANSFER_SIZE
uchar 				aTxBuffer[API_TRANSFER_SIZE];
uchar				rx_done = 0;
uchar				rx_active = 0;
ushort				cmd_req_in_progress = 0;

// Spectrum control publics
extern struct 		UI_SW	ui_sw;

// Driver communication
extern 			osMessageQId 			hApiMessage;

SPI_HandleTypeDef 	ApiSpiHandle;

uchar bc_mode = 1;

#ifdef API_DRIVER_USE_DMA
// Trap IRQs and HAL Events here:
//
// ---------------------------------------
// SPI DMA TX
void DMA1_Stream6_IRQHandler(void)
{
	HAL_DMA_IRQHandler(ApiSpiHandle.hdmatx);
}
//
// ---------------------------------------
// SPI DMA RX
void DMA1_Stream5_IRQHandler(void)
{
	HAL_DMA_IRQHandler(ApiSpiHandle.hdmarx);
}
//
// ---------------------------------------
// SPI IRQ
void SPI2_IRQHandler(void)
{
	HAL_SPI_IRQHandler(&ApiSpiHandle);
}
//
#ifdef API_DRIVER_DMA_FULL_DUPLEX
// Transfer finished from SPI HAL(full duplex)
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI2)
	{
		wTransferState = TRANSFER_COMPLETE;

		// Release DSP IRQ
		GPIOD->BSRRL = GPIO_PIN_5;
	}
}
#endif
//
#ifdef API_DRIVER_DMA_RX_ONLY
// Transfer finished from SPI HAL (RX only)
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI2)
	{
		wTransferState = TRANSFER_COMPLETE;

		// Release DSP IRQ
		GPIOD->BSRRL = GPIO_PIN_5;
	}
}
#endif
//
#ifdef API_DRIVER_DMA_TX_ONLY
// Transfer finished from SPI HAL (TX only)
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI2)
	{
		wTransferState = TRANSFER_COMPLETE;

		// Release DSP IRQ
		GPIOD->BSRRL = GPIO_PIN_5;
	}
}
#endif
//
// Error from SPI HAL
ushort spi_error;
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI2)
	{
		wTransferState = TRANSFER_ERROR;
		spi_error = hspi->ErrorCode;

		// Release DSP IRQ
		GPIOD->BSRRL = GPIO_PIN_5;
	}
}
#endif

#ifndef API_DRIVER_USE_DMA
void EXTI4_IRQHandler(void)
{
	ulong i,res;
	ulong tr_size = API_TRANSFER_SIZE;

	//if(bc_mode == 2) tr_size = 512;

	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);

		if(bc_mode == 2) return;

		rx_active = 1;	// lock out the api_ui_send_fast_cmd
		res = HAL_SPI_Receive(&ApiSpiHandle,(uint8_t*)DmaInBuffer, tr_size, 5000);
		rx_active = 0; 	// release lock

		if(res == HAL_OK)
		{
			// On success, copy to local buffer and set flag for further processing
			// without stalling the IRQ
			if(!rx_done)
			{
				// Copy to another buffer to prevent hanging
      			for(i = 0; i < tr_size; i++)
      				aRxBuffer[i] = DmaInBuffer[i];

      			// Processing flag
      			rx_done = 1;
      		}
		}
	}
}
#endif

//void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
static void api_ui_spi_hw_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	// SPI clock on
	//
	// Setting of peripheral clock for the SPI is done in mchf_pro_board_system_clock_config()
	//
	__HAL_RCC_SPI2_CLK_ENABLE();

	// DMA clock on
	#ifdef API_DRIVER_USE_DMA
	__HAL_RCC_DMA1_CLK_ENABLE();
	//__HAL_RCC_DMA2_CLK_ENABLE();
	#endif

	#ifndef API_DRIVER_USE_DMA
	// Local IRQ(DSP CS) - PD4
	GPIO_InitStruct.Pin		  = GPIO_PIN_4;
	GPIO_InitStruct.Mode      = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	// CS interrupt
	HAL_NVIC_SetPriority(EXTI4_IRQn, 0x0F, 0x00);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	#endif

	// Local CS(DSP IRQ) - PD5
	GPIO_InitStruct.Pin 	= GPIO_PIN_5;
	GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	// High by default
	GPIOD->BSRRL = GPIO_PIN_5;

	// SPI SCK
	GPIO_InitStruct.Pin       = GPIO_PIN_9;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// SPI MISO
	GPIO_InitStruct.Pin 	  = GPIO_PIN_2;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	// SPI MOSI
	GPIO_InitStruct.Pin		  = GPIO_PIN_1;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	#ifdef API_DRIVER_DMA_FULL_DUPLEX
	// Configure the DMA handler for Transmit
	hdma_tx.Instance                 = SPI2_TX_DMA_STREAM;

	hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
	hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
	hdma_tx.Init.Request             = SPI2_TX_DMA_REQUEST;
	hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_tx.Init.Mode                = DMA_NORMAL;
	hdma_tx.Init.Priority            = DMA_PRIORITY_MEDIUM;
	HAL_DMA_Init(&hdma_tx);

	// Associate the initialised DMA handle to the the SPI handle
	__HAL_LINKDMA(&ApiSpiHandle, hdmatx, hdma_tx);

	// Configure the DMA handler for Receive
	hdma_rx.Instance                 = SPI2_RX_DMA_STREAM;

	hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
	hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4;
	hdma_tx.Init.Request             = SPI2_RX_DMA_REQUEST;
	hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_rx.Init.Mode                = DMA_NORMAL;
	hdma_rx.Init.Priority            = DMA_PRIORITY_MEDIUM;
	HAL_DMA_Init(&hdma_rx);

	// Associate the initialised DMA handle to the the SPI handle
	__HAL_LINKDMA(&ApiSpiHandle, hdmarx, hdma_rx);

	// NVIC configuration for DMA transfer complete interrupt (SPI2_TX)
	HAL_NVIC_SetPriority(SPI2_DMA_TX_IRQn, 7, 7);
	HAL_NVIC_EnableIRQ	(SPI2_DMA_TX_IRQn);

	// NVIC configuration for DMA transfer complete interrupt (SPI2_RX)
	HAL_NVIC_SetPriority(SPI2_DMA_RX_IRQn, 7, 7);
	HAL_NVIC_EnableIRQ  (SPI2_DMA_RX_IRQn);

	// NVIC for SPI
	HAL_NVIC_SetPriority(SPI2_IRQn, 7, 7);
	HAL_NVIC_EnableIRQ  (SPI2_IRQn);
	#endif

	//
	// Following code is for testing purposes only
	//
	#ifdef API_DRIVER_DMA_RX_ONLY
	// Configure the DMA handler for Receive
	hdma_rx.Instance                 = SPI2_RX_DMA_STREAM;

	hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
	hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4;
	hdma_tx.Init.Request             = SPI2_RX_DMA_REQUEST;
	hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_rx.Init.Mode                = DMA_NORMAL;
	hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
	HAL_DMA_Init(&hdma_rx);

	// Associate the initialised DMA handle to the the SPI handle
	__HAL_LINKDMA(&ApiSpiHandle, hdmarx, hdma_rx);

	// NVIC configuration for DMA transfer complete interrupt (SPI2_RX)
	HAL_NVIC_SetPriority(SPI2_DMA_RX_IRQn, 7, 7);
	HAL_NVIC_EnableIRQ  (SPI2_DMA_RX_IRQn);

	// NVIC for SPI
	HAL_NVIC_SetPriority(SPI2_IRQn, 7, 7);
	HAL_NVIC_EnableIRQ  (SPI2_IRQn);
	#endif
	//
	#ifdef API_DRIVER_DMA_TX_ONLY
	// Configure the DMA handler for Transmit
	hdma_tx.Instance                 = SPI2_TX_DMA_STREAM;

	hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
	hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
	hdma_tx.Init.Request             = SPI2_TX_DMA_REQUEST;
	hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_tx.Init.Mode                = DMA_NORMAL;
	hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
	HAL_DMA_Init(&hdma_tx);

	// Associate the initialised DMA handle to the the SPI handle
	__HAL_LINKDMA(&ApiSpiHandle, hdmatx, hdma_tx);

	// NVIC configuration for DMA transfer complete interrupt (SPI2_TX)
	HAL_NVIC_SetPriority(SPI2_DMA_TX_IRQn, 7, 7);
	HAL_NVIC_EnableIRQ	 (SPI2_DMA_TX_IRQn);

	// NVIC for SPI
	HAL_NVIC_SetPriority(SPI2_IRQn, 7, 7);
	HAL_NVIC_EnableIRQ  (SPI2_IRQn);
	#endif
}

// Called by main(), instead of local init!!!
// due to H7 bug..
void api_driver_hw_init(void)
{
	ulong i;

	#ifdef API_DRIVER_USE_DMA
	// Clear buffer
	for(i = 0; i < API_DMA_RX_B_SIZE; i++)
		ucDmaTxBuffer[i] = 0x7E;
	#endif

	// GPIOs, IRQ and DMA
	api_ui_spi_hw_init();

	// Actual SPI
	ApiSpiHandle.Instance               = SPI2;
	//
	ApiSpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
	ApiSpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	ApiSpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	ApiSpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	ApiSpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	ApiSpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	ApiSpiHandle.Init.CRCPolynomial     = 7;
	ApiSpiHandle.Init.CRCLength        	= SPI_CRC_LENGTH_8BIT;
	ApiSpiHandle.Init.NSS               = SPI_NSS_SOFT;
	ApiSpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;

	#ifndef API_DRIVER_USE_DMA
	ApiSpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	ApiSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	ApiSpiHandle.Init.Mode 			 	= SPI_MODE_SLAVE;
	#endif

	#ifdef API_DRIVER_DMA_FULL_DUPLEX
	ApiSpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	ApiSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	ApiSpiHandle.Init.Mode 			 	= SPI_MODE_MASTER;
	#endif

	#ifdef API_DRIVER_DMA_TX_ONLY
	ApiSpiHandle.Init.Direction         = SPI_DIRECTION_2LINES_TXONLY;
	ApiSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	ApiSpiHandle.Init.Mode 			 	= SPI_MODE_MASTER;
	#endif

	#ifdef API_DRIVER_DMA_RX_ONLY
	ApiSpiHandle.Init.Direction         = SPI_DIRECTION_2LINES_RXONLY;
	ApiSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	ApiSpiHandle.Init.Mode 			 	= SPI_MODE_MASTER;
	//
	#if 0
	ApiSpiHandle.Init.FifoThreshold 				= SPI_FIFO_THRESHOLD_01DATA;
	ApiSpiHandle.Init.TxCRCInitializationPattern 	= SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	ApiSpiHandle.Init.RxCRCInitializationPattern	= SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	ApiSpiHandle.Init.MasterSSIdleness 				= SPI_MASTER_SS_IDLENESS_00CYCLE;
	ApiSpiHandle.Init.MasterInterDataIdleness 		= SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	ApiSpiHandle.Init.MasterReceiverAutoSusp 		= SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	ApiSpiHandle.Init.MasterKeepIOState 			= SPI_MASTER_KEEP_IO_STATE_DISABLE;
	ApiSpiHandle.Init.IOSwap 						= SPI_IO_SWAP_DISABLE;
	#endif
	#endif

	HAL_SPI_Init(&ApiSpiHandle);
}

//
// Non-blocking implementation
// Start exchange, end in callbacks
//
static void api_ui_dma_exchange(void)
{
#ifdef API_DRIVER_USE_DMA
	ulong i;

	// Generate DSP IRQ(pin restore in HAL_SPI_TxRxCpltCallback())
	GPIOD->BSRRH = GPIO_PIN_5;

	// Small delay (50 uS) to allow DMA in DSP to setup
	for(i = 0; i < 10000; i++)
		__asm(".word 0x46C046C0");

	// Invalidate D-cache before reception
	// Make sure the address is 32-byte aligned and add 32-bytes to length, in case it overlaps cache line
	#ifndef H7_DISABLE_DATA_CACHE
	SCB_InvalidateDCache_by_Addr((uint32_t*)(((uint32_t)ucDmaRxBuffer) & ~(uint32_t)0x1F), API_DMA_RX_B_SIZE+32);
	SCB_CleanDCache_by_Addr		((uint32_t*)(((uint32_t)ucDmaTxBuffer) & ~(uint32_t)0x1F), API_DMA_RX_B_SIZE+32);
	#endif

	// Default exchange mode
	#ifdef API_DRIVER_DMA_FULL_DUPLEX
	HAL_SPI_TransmitReceive_DMA(&ApiSpiHandle,ucDmaTxBuffer,ucDmaRxBuffer,API_DMA_RX_B_SIZE);
	#endif
	// Only for testing
	#ifdef API_DRIVER_DMA_RX_ONLY
	HAL_SPI_Receive_DMA(&ApiSpiHandle,ucDmaRxBuffer,API_DMA_RX_B_SIZE);
	#endif
	// Only for testing
	#ifdef API_DRIVER_DMA_TX_ONLY
	HAL_SPI_Transmit_DMA(&ApiSpiHandle,ucDmaTxBuffer,API_DMA_RX_B_SIZE);
	#endif
	//
#endif
}

static void api_ui_send_spi(void)
{
	#ifndef API_DRIVER_USE_DMA
	// Generate DSP IRQ
	GPIOD->BSRRH = GPIO_PIN_5;
	__asm(".word 0x46C046C0");

	HAL_SPI_Transmit(&ApiSpiHandle,(uint8_t*)aTxBuffer, 16, 1000);

	__asm(".word 0x46C046C0");
	GPIOD->BSRRL = GPIO_PIN_5;
	#endif

	#ifdef API_DRIVER_USE_DMA
	ushort i;

	for(i = 0; i < 16; i++)
		ucDmaTxBuffer[i] = aTxBuffer[i];
	#endif
}

// Directly send OS messages to DSP
static void api_ui_send_spi_a(ulong *msg)
{
	struct APIMessage *api_msg = (struct APIMessage *)msg;
	uchar  i;

	#ifdef API_UI_ALLOW_DEBUG
	printf("api_ui_send_spi_a,msg: 0x%02x\r\n",api_msg->usMessageID);
	#endif

	// DSP command
	aTxBuffer[0x00] = (api_msg->usMessageID >>   8);
	aTxBuffer[0x01] = (api_msg->usMessageID & 0xFF);

	// Trap broadcast mode changes
	if(api_msg->usMessageID == API_BROADCAST_MODE)
		bc_mode = api_msg->ucData[0];

	// Add payload, if present in the message
	if((api_msg->ucPayload) && (api_msg->ucPayload < (API_MAX_PAYLOAD + 1)))
	{
		for(i = 0; i < API_MAX_PAYLOAD; i++)
		{
			if(i < api_msg->ucPayload)
				aTxBuffer[i + 2] = api_msg->ucData[i];
			else
				aTxBuffer[i + 2] = 0;
		}
	}

	// Generate DSP IRQ
	GPIOD->BSRRH = GPIO_PIN_5;
	__asm(".word 0x46C046C0");

	HAL_SPI_Transmit(&ApiSpiHandle,(uint8_t*)aTxBuffer, 16, 1000);

	// Release DSP IRQ
	__asm(".word 0x46C046C0");
	GPIOD->BSRRL = GPIO_PIN_5;
}

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

#ifdef API_DRIVER_USE_DMA
static void api_driver_handle_dma(void)
{
	// DMA handler state machine
	switch(wTransferState)
	{
		// -------------------------------------------------
		// Pending DMA transfer
		case TRANSFER_WAIT:
			//printf("..");		// set thread delay to 1mS to catch this case!
			break;

		// -------------------------------------------------
		// DMA complete
		case TRANSFER_COMPLETE:
		{
			wTransferState = TRANSFER_WAIT;

			printf("reload\r\n");

			// Reload
			api_ui_dma_exchange();

			break;
		}

		// -------------------------------------------------
		// DMA error
		case TRANSFER_ERROR:
		{
			wTransferState = TRANSFER_COMPLETE;

			printf("spi error %d\r\n",spi_error);

			// Clear error
			spi_error = 0;

			break;
		}

		default:
			break;
	}
	OsDelayMs(20);
}
#endif

static void api_driver_worker(void)
{
	#ifndef API_DRIVER_USE_DMA
	api_ui_send_fast_cmd_a();
	api_ui_send_fast_cmd();
	api_ui_process_broadcast();
	#endif

	#ifdef API_DRIVER_USE_DMA
	api_driver_handle_dma();
	#endif

	#ifdef API_DRIVER_USE_DMA___
	// -------------------------------------------
	ushort i;

	if(wTransferState != TRANSFER_WAIT)
	{
		//api_ui_send_fast_cmd();
		//api_ui_process_broadcast();
	}
	//
	if(wTransferState == TRANSFER_COMPLETE)
	{
		//for(i = 0; i < 16; i++)
		//	ucDmaSafeBuffer[i] = 0;
	}
	//
	spi_error = 0;
	api_ui_dma_exchange();
	if(spi_error)
		printf("spi error %d\r\n",spi_error);
	else
		printf("spi success\r\n");
	//
	OsDelayMs(100);
	// -------------------------------------------
	#endif
}
#endif


#ifdef CONTEXT_ICC

#define RPMSG_CHAN_NAME              "stm32_icc_service"

static  uint32_t	 			message = 0;
static volatile int	 			message_received = 0;
static volatile int 			service_created = 0;
//static volatile unsigned int 	received_data;
static struct rpmsg_endpoint 	rp_endpoint;

uchar 				aRxBuffer[2048];

// Spectrum control publics
extern struct 		UI_SW	ui_sw;

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
	if(skip_bc < 10) return;
	skip_bc = 0;
	// ----------------------------

	// Data valid ?
	if((aRxBuffer[0] != 0x12) || (aRxBuffer[1] != 0x34) || (aRxBuffer[298] != 0x55) || (aRxBuffer[299] != 0xAA))
	{
		//printf("sig: %02x %02x\r\n",aRxBuffer[0],aRxBuffer[1]);
		goto exit_anyway;
	}

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
uchar icc_proc_cmd_xchange(uchar cmd)
{
	int32_t status;

	message = cmd;

	// Send the massage to the remote CPU
	status = OPENAMP_send(&rp_endpoint, &message, sizeof(message));
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
	OPENAMP_Wait_EndPointready(&rp_endpoint);

	// Callback activated ?
	if(service_created == 0)
	{
		printf("error service creation!\r\n");
		return 2;
	}

	//printf("send first message...\r\n");

	// Start I2S process
	if(icc_proc_cmd_xchange(ICC_START_I2S_PROC))
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
	if(icc_proc_cmd_xchange(ICC_TOGGLE_LED) != 0)
		tsu.dsp_alive = 0;								// Ack to UI lost comm to DSP
	else
		tsu.dsp_blinker = !tsu.dsp_blinker;				// Fill blinker

	ul_blink = 0;
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

	if((ulNotif) && (ulNotificationValue))
	{
		printf("command to route to M4: %02x\r\n", ulNotificationValue);

		// Send command to M4 from any task
		icc_proc_cmd_xchange(ulNotificationValue & 0xFF);
	}
	else
	{
		// Just a gimmick really
		icc_proc_blink_remote_led();

		// Initiate broadcast - 100Hz FFT buffer refresh
		if(icc_proc_cmd_xchange(ICC_BROADCAST) == 0)
			api_ui_process_broadcast();
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
